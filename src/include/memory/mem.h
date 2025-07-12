#ifndef MEMORY_H
#define MEMORY_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "utils/types.h"

typedef struct Stack {
  uintptr_t sp;
  uintptr_t start;
  size_t size;
} Stack;

typedef struct Memory {
  void* memory;
  size_t size;
  bool initialized;
} Memory;

Result initialize_memory(Memory* memory);

#endif
