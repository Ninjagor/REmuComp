#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <stdint.h>

#define SPRITESHEET_SPRITE_COUNT 36
#define SPRITESHEET_SPRITE_SIZE 8  // 8 bytes per sprite (8x8 pixels)

const uint8_t spritesheet[128][8];

void load_spritesheet_to_memory(uint8_t *memory);

#endif
