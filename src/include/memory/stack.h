#ifndef STACK_H
#define STACK_H

#include "memory/mem.h"
#include <stdint.h>

Result push(Memory* memory, Stack* stack, uint16_t value);
Result pop(Memory* memory, Stack* stack, uint16_t* value);

#endif
