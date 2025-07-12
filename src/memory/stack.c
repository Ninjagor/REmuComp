#include "memory/mem.h"
#include "utils/types.h"
#include "memory/stack.h"
#include <stdint.h>

Result push(Memory* memory, Stack* stack, uint16_t value) {
    if (stack->sp < 2) return ERROR; 

    stack->sp -= 2;
    uint8_t* mem = (uint8_t*)memory->memory;

    mem[stack->sp] = value & 0xFF;
    mem[stack->sp + 1] = (value >> 8) & 0xFF;

    return SUCCESS;
}

Result pop(Memory* memory, Stack* stack, uint16_t* value) {
    if (stack->sp > stack->start + stack->size - 2) return ERROR;

    uint8_t* mem = (uint8_t*)memory->memory;

    *value = mem[stack->sp] | (mem[stack->sp + 1] << 8);

    stack->sp += 2;

    return SUCCESS;
}
