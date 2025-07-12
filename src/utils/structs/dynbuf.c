#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "utils/structs/dynbuf.h"

void initBuffer(DynBuffer* buf, size_t initial_capacity) {
    buf->data = malloc(initial_capacity * sizeof(uint16_t));
    buf->size = 0;
    buf->capacity = initial_capacity;
}

void freeBuffer(DynBuffer* buf) {
    free(buf->data);
    buf->data = NULL;
    buf->size = 0;
    buf->capacity = 0;
}

void appendWord(DynBuffer* buf, uint16_t word) {
    if (buf->size + 1 > buf->capacity) {
        size_t new_capacity = buf->capacity * 2;
        uint16_t* new_data = realloc(buf->data, new_capacity * sizeof(uint16_t));
        if (!new_data) {
            return;
        }
        buf->data = new_data;
        buf->capacity = new_capacity;
    }
    buf->data[buf->size++] = word;
}

void appendWords(DynBuffer* buf, const uint16_t* words, size_t len) {
    while (buf->size + len > buf->capacity) {
        size_t new_capacity = buf->capacity * 2;
        uint16_t* new_data = realloc(buf->data, new_capacity * sizeof(uint16_t));
        if (!new_data) {
            return;
        }
        buf->data = new_data;
        buf->capacity = new_capacity;
    }
    memcpy(buf->data + buf->size, words, len * sizeof(uint16_t));
    buf->size += len;
}
