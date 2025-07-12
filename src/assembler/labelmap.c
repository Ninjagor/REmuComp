#include "assembler/labelmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void init_label_map(LabelMap* map) {
  map->count = 0;
  map->capacity = 8;
  map->entries = malloc(sizeof(LabelEntry) * map->capacity);
}

void free_label_map(LabelMap* map) {
  for (size_t i = 0; i < map->count; i++)
    free(map->entries[i].label);
  free(map->entries);
}

bool add_label(LabelMap* map, const char* label, uint16_t address) {
  for (size_t i = 0; i < map->count; i++) {
    if (strcmp(map->entries[i].label, label) == 0) {
      printf("\nCOMPILATION ERROR: Label redefined: %s\n", label);
      return false;
    }
  }

  if (map->count >= map->capacity) {
    map->capacity *= 2;
    map->entries = realloc(map->entries, sizeof(LabelEntry) * map->capacity);
  }

  map->entries[map->count].label = strdup(label);
  map->entries[map->count].address = address;
  map->count++;

  return true;
}

int get_label_address(LabelMap* map, const char* label) {
  for (size_t i = 0; i < map->count; i++) {
    if (strcmp(map->entries[i].label, label) == 0)
      return map->entries[i].address;
  }
  return -1;
}
