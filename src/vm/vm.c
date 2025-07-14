#include "vm/vm.h"
#include "constants.h"
#include "cpu/cpu.h"
#include "graphics/spritesheet.h"
#include "graphics/render.h"
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
#include <unistd.h> 

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

// int start_x = (96 - 8) / 2;
// int start_y = (64 - 8) / 2;
// const uint8_t* sprite = &spritesheet[10 * SPRITESHEET_SPRITE_SIZE];
//
// for (int row = 0; row < 8; row++) {
//     uint8_t bits = sprite[row];
//     for (int col = 0; col < 8; col++) {
//         if (bits & (1 << (7 - col))) {
//             int x = start_x + col;
//             int y = start_y + row;
//             vm->vram[y * 96 + x] = 1;
//         }
//     }
// }

  return SUCCESS;
}

void load_sprite_table_debug(FILE* f) {
    printf("\n\n=== Sprite Table Debug Output ===\n");

    int ch;
    size_t byte_count = 0;
    uint8_t sprite[8];

    while (true) {
        size_t i = 0;
        for (; i < 8; i++) {
            ch = fgetc(f);
            if (ch == EOF) break;
            sprite[i] = (uint8_t)ch;
        }
        if (i == 0) break; 

        printf("Sprite %zu: ", byte_count / 8);
        for (size_t j = 0; j < i; j++) {
            printf("%02X ", sprite[j]);
        }
        printf("\n");

        if (i < 8) break;
        byte_count += i;
    }

    printf("=== End of Sprite Table ===\n");
}

void load_sprite_table(VM* vm, FILE* f) {
    vm->sprite_table = malloc(MAX_SPRITES * SPRITE_SIZE * sizeof(uint8_t));
    if (!vm->sprite_table) {
        printf("Failed to allocate sprite table\n");
        vm->sprite_count = 0;
        return;
    }
    vm->sprite_count = 0;

    while (vm->sprite_count < MAX_SPRITES) {
        size_t read_bytes = fread(vm->sprite_table + vm->sprite_count * SPRITE_SIZE, 1, SPRITE_SIZE, f);
        if (read_bytes == 0) break; // EOF
        if (read_bytes < SPRITE_SIZE) {
            printf("Warning: incomplete sprite data\n");
            break;
        }
        vm->sprite_count++;
    }
}

// void load_sprite_table(VM* vm, FILE* f) {
//     vm->sprite_table = malloc(MAX_SPRITES * 8 * sizeof(uint8_t));
//     if (!vm->sprite_table) {
//         printf("Failed to allocate sprite table\n");
//         vm->sprite_count = 0;
//         return;
//     }
//     vm->sprite_count = 0;
//
//     while (vm->sprite_count < MAX_SPRITES) {
//         uint8_t sprite[8];
//         size_t read_bytes = fread(sprite, 1, 8, f);
//         if (read_bytes == 0) break; // EOF
//         if (read_bytes < 8) {
//             printf("Warning: incomplete sprite data\n");
//             break;
//         }
//
//         memcpy(&vm->sprite_table[vm->sprite_count * 8], sprite, 8);
//         vm->sprite_count++;
//     }
// }

const uint8_t* get_sprite_from_vm(VM* vm, uint16_t idx) {
    if (idx >= vm->sprite_count) return NULL;

    const uint8_t* sprite = &vm->sprite_table[idx * SPRITE_SIZE];

    // printf("\nSprite %u: \n", idx);
    for (int i = 0; i < SPRITE_SIZE; i++) {
        // printf("%02X ", sprite[i]);
    }
    // printf("\n");

    return sprite;
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
        int ch;

        while ((ch = fgetc(f)) != EOF) {
            if (ch == 0) break;  // null terminator
            if (i < MAX_STRING_LENGTH - 1) {
                buffer[i++] = (char)ch;
            }
        }
        buffer[i] = '\0';

        if (i == 0) break;  // no more strings

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

    if (fread(bytes, 1, file_size, f) != file_size) {
        free(bytes);
        fclose(f);
        return ERROR;
    }

    size_t len_words = file_size / 2;
    size_t delimiter_index = len_words;

    // Find first delimiter (0xFFFF 4 times)
    for (size_t i = 0; i < len_words - 3; i++) {
        uint16_t w0 = bytes[i * 2] | (bytes[i * 2 + 1] << 8);
        uint16_t w1 = bytes[(i + 1) * 2] | (bytes[(i + 1) * 2 + 1] << 8);
        uint16_t w2 = bytes[(i + 2) * 2] | (bytes[(i + 2) * 2 + 1] << 8);
        uint16_t w3 = bytes[(i + 3) * 2] | (bytes[(i + 3) * 2 + 1] << 8);

        if (w0 == 0xFFFF && w1 == 0xFFFF && w2 == 0xFFFF && w3 == 0xFFFF) {
            delimiter_index = i;
            break;
        }
    }

    if (delimiter_index == len_words) {
        printf("Error: delimiter not found\n");
        free(bytes);
        fclose(f);
        return ERROR;
    }

    size_t program_bytes = delimiter_index * 2;
    size_t data_bytes = (file_size > program_bytes + 8) ? (file_size - program_bytes - 8) : 0;

    if (program_bytes > PROGRAM_MAX) {
        free(bytes);
        fclose(f);
        return ERROR;
    }

    uint8_t* ram_bytes = (uint8_t*)vm->ram.memory;

    // Copy program bytes to RAM
    memcpy(ram_bytes + PROGRAM_START, bytes, program_bytes);

    // Copy data bytes to RAM data segment if present
    if (data_bytes > 0) {
        memcpy(ram_bytes + DATA_SEGMENT_START,
               bytes + program_bytes + 8,
               data_bytes);
    }

    long string_start = program_bytes + 8;

    // Find second delimiter (0xFFFF 4 times) after first delimiter + 4 words
    long sprite_start = -1;
    for (size_t i = delimiter_index + 4; i < len_words - 3; i++) {
        uint16_t w0 = bytes[i * 2] | (bytes[i * 2 + 1] << 8);
        uint16_t w1 = bytes[(i + 1) * 2] | (bytes[(i + 1) * 2 + 1] << 8);
        uint16_t w2 = bytes[(i + 2) * 2] | (bytes[(i + 2) * 2 + 1] << 8);
        uint16_t w3 = bytes[(i + 3) * 2] | (bytes[(i + 3) * 2 + 1] << 8);

        if (w0 == 0xFFFF && w1 == 0xFFFF && w2 == 0xFFFF && w3 == 0xFFFF) {
            sprite_start = (i + 4) * 2;  // Corrected
            break;
        }
    }

    if (sprite_start == -1) {
        printf("Error: second delimiter not found\n");
        free(bytes);
        fclose(f);
        return ERROR;
    }

    // Load string table from file starting at string_start
    fseek(f, string_start, SEEK_SET);
    load_string_table(vm, f);

    // Debug and load sprite table
    // fseek(f, sprite_start, SEEK_SET);
    // load_sprite_table_debug(f);

    fseek(f, sprite_start, SEEK_SET);
    load_sprite_table(vm, f);

    free(bytes);
    fclose(f);
    return SUCCESS;
}


Result run_program(VM* vm) {
    const useconds_t delay_us = 1000000 / VM_OPS_PER_SECOND;
    int timer_tick_counter = 0;
    const int ticks_per_timer_decrement = VM_OPS_PER_SECOND / 60;

    while (true) {
        timer_tick_counter++;

        InterruptFlag interrupt = vm->cpu.flags.program_interrupt;
        if (interrupt != RUNNING) {
            if (interrupt == EFINISH) {
                // printf("\nREmu VM Exited with an error.\n");
                return ERROR;
            } else if (interrupt == SFINISH) {
                // printf("\nREmu VM Exited successfully.\n");
                return SUCCESS;
            }
            break;
        }

        uint16_t words[4];
        uint8_t* memory = (uint8_t*)vm->ram.memory;

        for (int i = 0; i < 4; i++) {
            words[i] = memory[vm->cpu.pc + i * 2] | (memory[vm->cpu.pc + i * 2 + 1] << 8);
        }

        if (vm->cpu.flags.graphics_initialized == AWAITING_INITIALIZATION) {
          init_display();
          vm->cpu.flags.graphics_initialized = INITIALIZED;
          render(vm);
        }

        if (vm->cpu.flags.input_poll_flag == 1) {
          if (vm->cpu.flags.graphics_initialized != INITIALIZED) {
              printf("\n REmuVM - Error - attempted to render without initializing display. \n");
              return ERROR; 
          }
          
          poll_keys(vm);
          vm->cpu.flags.input_poll_flag = 0;
        }

        if (vm->cpu.flags.draw_flag == 1) {
          if (vm->cpu.flags.graphics_initialized != INITIALIZED) {
              printf("\n REmuVM - Error - attempted to render without initializing display. \n");
              return ERROR; 
          }
          render(vm);
          vm->cpu.flags.draw_flag = 0;
        }


        if (timer_tick_counter >= ticks_per_timer_decrement) {
            timer_tick_counter = 0;

            if (vm->cpu.flags.delay_timer > 0) vm->cpu.flags.delay_timer--;
            if (vm->cpu.flags.sound_timer > 0) {
                vm->cpu.flags.sound_timer--;
                if (vm->cpu.flags.sound_timer == 0) {
                    // stop_beep();
                }
            }
        }

        //
        // printf("\033[2J");
        //
        // printf("\nCPU Register Peek: \n");
        // for (int i = 0; i < CPU_REGISTER_COUNT; i++) {
        //   char* p = "";
        //   if (i < 10) {
        //     p = "0";
        //   }
        //   printf("R%s%i 0x%04X ", p, i, vm->cpu.registers[i].value);
        //   if ((i+1)%4 == 0) {
        //     printf("\n");
        //   }
        // }

        interpret_op(vm, words);


        usleep(delay_us);
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


    if (vm->sprite_table) {
        free(vm->sprite_table);
        vm->sprite_table = NULL;
    }
}
