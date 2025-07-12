#include "memory/mem.h"
#include "utils/types.h"
#include "memory/stack.h"
#include <stdint.h>

Result push(Memory* memory, Stack* stack, uint16_t value) {
    uintptr_t sp_offset = stack->sp - (uintptr_t)memory->memory;

    // if (sp_offset < 2) return ERROR; 
    if (stack->sp - 2 < stack->start) return ERROR;

    sp_offset -= 2;

    uint8_t* mem = (uint8_t*)memory->memory;

    mem[sp_offset] = value & 0xFF;
    mem[sp_offset + 1] = (value >> 8) & 0xFF;

    stack->sp = (uintptr_t)memory->memory + sp_offset;

    return SUCCESS;
}

Result pop(Memory* memory, Stack* stack, uint16_t* value) {
    uintptr_t sp_offset = stack->sp - (uintptr_t)memory->memory;

    if (stack->sp >= stack->start + stack->size) return ERROR; 

    uint8_t* mem = (uint8_t*)memory->memory;

    *value = mem[sp_offset] | (mem[sp_offset + 1] << 8);

    sp_offset += 2;
    stack->sp = (uintptr_t)memory->memory + sp_offset;

    return SUCCESS;
}
