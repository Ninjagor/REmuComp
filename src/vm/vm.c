#include "vm/vm.h"
#include "constants.h"
#include "cpu/cpu.h"
#include "interpreter/interpreter.h"
#include "memory/mem.h"
#include "parser/parser.h"
#include "utils/fs/fs.h"
#include "utils/types.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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

Result load_program(VM* vm, const char* filepath) {
  size_t len;
  uint8_t* bytes = read_file_to_buffer(filepath, &len);
  if (!bytes) {
      printf("\nError reading binary file\n");
      perror("\nREmu VM encountered a fatal error\n");
      return ERROR;
  }

  if (len > PROGRAM_MAX) {
    printf("\nProgram exceeding RAM Limits\n");
    perror("\nREmu VM encountered a fatal error\n");
    return ERROR;
  }

  for (size_t i = 0; i < len; i++) {
      ((uint8_t*)vm->ram.memory)[PROGRAM_START + i] = bytes[i];
  }

  free(bytes);

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
