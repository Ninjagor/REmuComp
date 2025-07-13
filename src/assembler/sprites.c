#include "assembler/sprites.h"
#include <stdlib.h>
#include <string.h>

void init_sprite_table(SpriteTable* st) {
    st->sprites = NULL;
    st->count = 0;
    st->capacity = 0;
}

int add_sprite(SpriteTable* st, uint8_t* sprite_data) {
    if (st->count == st->capacity) {
        size_t new_cap = st->capacity ? st->capacity * 2 : 8;
        uint8_t** tmp = realloc(st->sprites, new_cap * sizeof(uint8_t*));
        if (!tmp) return -1;
        st->sprites = tmp;
        st->capacity = new_cap;
    }

    st->sprites[st->count] = malloc(8);
    if (!st->sprites[st->count]) return -1;
    memcpy(st->sprites[st->count], sprite_data, 8);

    return (int)(st->count++);
}

void free_sprite_table(SpriteTable* st) {
    for (size_t i = 0; i < st->count; i++) {
        free(st->sprites[i]);
    }
    free(st->sprites);
    st->sprites = NULL;
    st->count = 0;
    st->capacity = 0;
}

void write_sprite_table(FILE* f, SpriteTable* st) {
    for (size_t i = 0; i < st->count; i++) {
        fwrite(st->sprites[i], sizeof(uint8_t), 8, f);
    }
}
