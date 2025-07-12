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
} VM;

Result initialize_vm(VM* vm);

Result load_program(VM* vm, const char* filepath);


Result run_program(VM* vm);

#endif
