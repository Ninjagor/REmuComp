#include "vm/vm.h"
#include "constants.h"
#include "cpu/cpu.h"
#include "graphics/spritesheet.h"
#include "interpreter/interpreter.h"
#include "memory/mem.h"
#include "parser/parser.h"
#include "utils/fs/fs.h"
#include "utils/types.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Result initialize_vm(VM *vm) {
  vm->ram.size = RAM_SIZE;
  if (initialize_memory(&vm->ram) == ERROR) {
    printf("RAM Initialization failed \n");
    perror("REmu VM Failed to intitialize\n");
    return ERROR;
  }

  vm->stack.start = (uintptr_t)vm->ram.memory + STACK_START;
  vm->stack.sp = vm->stack.start + STACK_SIZE;
  vm->stack.size = STACK_SIZE;

  load_spritesheet_to_memory(vm->ram.memory);


  // vm->stack.start = STACK_START;        // offset, not absolute address
  // vm->stack.sp = vm->stack.start + STACK_SIZE;
  // vm->stack.size = STACK_SIZE;
  //
  vm->vram = (uint8_t*)vm->ram.memory + VRAM_START;
  vm->spritesheet = (uint8_t*)vm->ram.memory + SPRITESHEET_START;
  vm->data_segments = (uint8_t*)vm->ram.memory + DATA_SEGMENT_START;

  vm->cpu.pc = 0x00;

  for (int i = 0; i < CPU_REGISTER_COUNT; i++) {
    Register reg = {0};
    reg.register_id = i;
    reg.value = 0x0000;

    vm->cpu.registers[i] = reg;
  }

  vm->cpu.flags.program_interrupt = RUNNING;

  return SUCCESS;
}

void load_string_table(VM* vm, FILE* f) {
    vm->string_table = malloc(MAX_STRINGS * sizeof(char*));
    if (!vm->string_table) {
        printf("Failed to allocate string table\n");
        vm->string_count = 0;
        return;
    }
    vm->string_count = 0;

    while (true) {
        char buffer[MAX_STRING_LENGTH];
        int i = 0;
        uint16_t ch;

        while (fread(&ch, sizeof(uint16_t), 1, f) == 1) {
            if (ch == 0) break;
            if (i < MAX_STRING_LENGTH - 1) {
                buffer[i++] = (char)(ch & 0xFF);
            }
        }
        buffer[i] = '\0';

        if (i == 0) break;

        vm->string_table[vm->string_count] = strdup(buffer);
        if (!vm->string_table[vm->string_count]) {
            printf("Failed to duplicate string\n");
            break;
        }

        vm->string_count++;
        if (vm->string_count >= MAX_STRINGS) break;
    }
}

const char* get_string_from_vm(VM* vm, uint16_t idx) {
    if (idx >= vm->string_count) return NULL;
    return vm->string_table[idx];
}

Result load_program(VM* vm, const char* filepath) {
    FILE* f = fopen(filepath, "rb");
    if (!f) return ERROR;

    fseek(f, 0, SEEK_END);
    size_t file_size = ftell(f);
    rewind(f);

    uint8_t* bytes = malloc(file_size);
    if (!bytes) {
        fclose(f);
        return ERROR;
    }
    fread(bytes, 1, file_size, f);

    uint16_t* words = (uint16_t*)bytes;
    size_t len_words = file_size / 2;

    size_t delimiter_index = len_words;
    for (size_t i = 0; i < len_words; i++) {
        if (words[i] == 0xFFFF) {
            delimiter_index = i;
            break;
        }
    }

    size_t program_words = delimiter_index;
    size_t data_words = (delimiter_index == len_words) ? 0 : (len_words - delimiter_index - 1);

    if (program_words * 2 > PROGRAM_MAX) {
        free(bytes);
        fclose(f);
        return ERROR;
    }

    uint8_t* ram_bytes = (uint8_t*)vm->ram.memory;
    memcpy(ram_bytes + PROGRAM_START, bytes, program_words * 2);

    if (data_words > 0) {
        memcpy(ram_bytes + DATA_SEGMENT_START, bytes + (delimiter_index + 1) * 2, data_words * 2);
    }

    free(bytes);

    fseek(f, (delimiter_index + 1) * 2, SEEK_SET);
    load_string_table(vm, f);

    fclose(f);
    return SUCCESS;
}

Result run_program(VM* vm) {
  while (true) {
    InterruptFlag interrupt = vm->cpu.flags.program_interrupt;
    if (interrupt != RUNNING) {
      if (interrupt == EFINISH) {
        printf("\nREmu VM Exited with an error.\n");
      } else if (interrupt == SFINISH) {
        printf("\nREmu VM Exited successfully.\n");
      }
      break;
    }

    uint16_t words[4];
    uint8_t* memory = (uint8_t*)vm->ram.memory;

    for (int i = 0; i < 4; i++) {
      words[i] = memory[vm->cpu.pc + i * 2] | (memory[vm->cpu.pc + i * 2 + 1] << 8);
    }

    // printf("0x%04X \n", words[0]);

    interpret_op(vm, words);
  }

  return SUCCESS;
}

void cleanup_vm(VM *vm) {
    if (vm->string_table) {
        for (size_t i = 0; i < vm->string_count; i++) {
            free(vm->string_table[i]);
        }
        free(vm->string_table);
        vm->string_table = NULL;
        vm->string_count = 0;
    }

    if (vm->ram.memory) {
        free(vm->ram.memory);
        vm->ram.memory = NULL;
    }
}
