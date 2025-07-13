#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

typedef struct {
    uint8_t** sprites;
    size_t count;
    size_t capacity;
} SpriteTable;

void init_sprite_table(SpriteTable* st);
int add_sprite(SpriteTable* st, uint8_t* sprite_data); // expects 8 bytes
void free_sprite_table(SpriteTable* st);
void write_sprite_table(FILE* f, SpriteTable* st);
