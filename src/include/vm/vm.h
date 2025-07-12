#ifndef VM_H
#define VM_H

#include "cpu/cpu.h"
#include "constants.h"
#include "memory/mem.h"
#include <stdint.h>

typedef struct VM {
  // cpu
  CPU cpu;

  // RAM
  Memory ram;

  // specific RAM partitions
  Stack stack;
  uint8_t* vram;
  uint8_t* spritesheet;
  uint8_t* data_segments;

  // string table
  char** string_table;
  size_t string_count; 
} VM;

Result initialize_vm(VM* vm);

Result load_program(VM* vm, const char* filepath);

const char* get_string_from_vm(VM* vm, uint16_t idx);

Result run_program(VM* vm);

void cleanup_vm(VM *vm);

#endif
