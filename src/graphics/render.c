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
    ClearBackground(DARKGREEN);
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
    SetTargetFPS(60);
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
