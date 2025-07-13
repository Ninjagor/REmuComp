#include <raylib.h>
#include "graphics/spritesheet.h"
#include "vm/vm.h"

#define SPRITE_SIZE 8
#define SPRITES_PER_ROW 12
#define SCALE 8

#define WINDOW_WIDTH (96 * SCALE)
#define WINDOW_HEIGHT (64 * SCALE)

#define BLOOM_ALPHA 40

static void preprocess_screen(void) {
    // ClearBackground(DARKGREEN);
    ClearBackground(BLACK);
}

void poll_keys(VM* vm) {
    for (int i = 0; i < 16; i++) {
      vm->cpu.flags.keys[i] = false;
    }

    if (IsKeyDown(KEY_ZERO))  vm->cpu.flags.keys[0x0] = true;
    if (IsKeyDown(KEY_ONE))   vm->cpu.flags.keys[0x1] = true;
    if (IsKeyDown(KEY_TWO))   vm->cpu.flags.keys[0x2] = true;
    if (IsKeyDown(KEY_THREE)) vm->cpu.flags.keys[0x3] = true;
    if (IsKeyDown(KEY_FOUR))  vm->cpu.flags.keys[0x4] = true;
    if (IsKeyDown(KEY_FIVE))  vm->cpu.flags.keys[0x5] = true;
    if (IsKeyDown(KEY_SIX))   vm->cpu.flags.keys[0x6] = true;
    if (IsKeyDown(KEY_SEVEN)) vm->cpu.flags.keys[0x7] = true;
    if (IsKeyDown(KEY_EIGHT)) vm->cpu.flags.keys[0x8] = true;
    if (IsKeyDown(KEY_NINE))  vm->cpu.flags.keys[0x9] = true;
    if (IsKeyDown(KEY_A))     vm->cpu.flags.keys[0xA] = true;
    if (IsKeyDown(KEY_B))     vm->cpu.flags.keys[0xB] = true;
    if (IsKeyDown(KEY_C))     vm->cpu.flags.keys[0xC] = true;
    if (IsKeyDown(KEY_D))     vm->cpu.flags.keys[0xD] = true;
    if (IsKeyDown(KEY_E))     vm->cpu.flags.keys[0xE] = true;
    if (IsKeyDown(KEY_F))     vm->cpu.flags.keys[0xF] = true;
}

static void draw_sprite_glow(int x, int y) {
    DrawRectangle(x, y, SCALE, SCALE, WHITE);
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx || dy) {
                DrawRectangle(x + dx, y + dy, SCALE, SCALE,
                              (Color){255, 255, 255, BLOOM_ALPHA});
            }
        }
    }
}

void init_display(void) {
    SetTraceLogLevel(LOG_NONE);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "REmu - VM");
    // SetTargetFPS(120);
}

void render(VM* vm) {
    BeginDrawing();
    preprocess_screen();

    for (int y = 0; y < 64; y++) {
        for (int x = 0; x < 96; x++) {
            size_t i = y * 96 + x;
            if (vm->vram[i])
                draw_sprite_glow(x * SCALE, y * SCALE);
        }
    }

    EndDrawing();
}
