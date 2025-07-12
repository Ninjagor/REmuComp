#include "memory/mem.h"
#include "utils/types.h"
#include <stdint.h>

Result push(Memory* memory, Stack* stack, uint8_t value) {
  // stack full
  if (stack->sp == stack->start) {
    return ERROR;
  }

  stack->sp--;
  ((uint8_t*)memory->memory)[stack->sp] = value;

  return SUCCESS;
}

Result pop(Memory* memory, Stack* stack, uint8_t* value) {
  // stack empty/invalid SP
  if (stack->sp == stack->start+stack->size) {
    return ERROR;
  }

  *value = ((uint8_t*)memory->memory)[stack->sp];
  stack->sp++;

  return SUCCESS;
}
