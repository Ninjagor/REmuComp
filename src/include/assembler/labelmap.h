#pragma once

#include "utils/types.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
  char* label;
  uint16_t address;
} LabelEntry;

typedef struct {
  LabelEntry* entries;
  size_t count;
  size_t capacity;
} LabelMap;

void init_label_map(LabelMap* map);
void free_label_map(LabelMap* map);
bool add_label(LabelMap* map, const char* label, uint16_t address);
int get_label_address(LabelMap* map, const char* label);
