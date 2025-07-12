#ifndef STACK_H
#define STACK_H

#include "memory/mem.h"
#include <stdint.h>

uint8_t pop(Memory* memory, Stack* stack);

Result push(Memory* memory, Stack* stack, uint8_t value);

#endif
