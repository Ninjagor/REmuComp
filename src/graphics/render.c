#include <raylib.h>
#include "graphics/spritesheet.h"
#include "vm/vm.h"
#include "fonts/chicago_font.h"

#define SCALE 8
#define SCREEN_WIDTH 96
#define SCREEN_HEIGHT 64
#define WINDOW_WIDTH (SCREEN_WIDTH * SCALE + 64)
#define WINDOW_HEIGHT (SCREEN_HEIGHT * SCALE + 240)

#define SCREEN_X ((WINDOW_WIDTH - SCREEN_WIDTH * SCALE) / 2)
#define SCREEN_Y 60
#define KEYPAD_TOP (SCREEN_Y + SCREEN_HEIGHT * SCALE + 24)

#define BLOOM_ALPHA 40
#define BUTTON_SIZE 32
#define BUTTON_GAP 4

const Color APP_BG = (Color){163, 158, 130, 255};
const Color BEZEL = (Color){0, 0, 0, 255};
const Color SCREEN_BG = (Color){26, 37, 31, 255};
const Color KEYPAD_BG = (Color){26, 20, 10, 100};
const Color BUTTON_BG = (Color){163, 158, 130, 255};

const char* KEYMAP[16] = {
    "1","2","3","C",
    "4","5","6","D",
    "7","8","9","E",
    "A","0","B","F"
};

static Font retroFont;

void poll_keys(VM* vm) {
    for (int i = 0; i < 16; i++) vm->cpu.flags.keys[i] = false;

    if (IsKeyDown(KEY_ONE))   vm->cpu.flags.keys[0x0] = true;
    if (IsKeyDown(KEY_TWO))   vm->cpu.flags.keys[0x1] = true;
    if (IsKeyDown(KEY_THREE)) vm->cpu.flags.keys[0x2] = true;
    if (IsKeyDown(KEY_FOUR))  vm->cpu.flags.keys[0x3] = true;

    if (IsKeyDown(KEY_Q))     vm->cpu.flags.keys[0x4] = true;
    if (IsKeyDown(KEY_W))     vm->cpu.flags.keys[0x5] = true;
    if (IsKeyDown(KEY_E))     vm->cpu.flags.keys[0x6] = true;
    if (IsKeyDown(KEY_R))     vm->cpu.flags.keys[0x7] = true;

    if (IsKeyDown(KEY_A))     vm->cpu.flags.keys[0x8] = true;
    if (IsKeyDown(KEY_S))     vm->cpu.flags.keys[0x9] = true;
    if (IsKeyDown(KEY_D))     vm->cpu.flags.keys[0xA] = true;
    if (IsKeyDown(KEY_F))     vm->cpu.flags.keys[0xB] = true;

    if (IsKeyDown(KEY_Z))     vm->cpu.flags.keys[0xC] = true;
    if (IsKeyDown(KEY_X))     vm->cpu.flags.keys[0xD] = true;
    if (IsKeyDown(KEY_C))     vm->cpu.flags.keys[0xE] = true;
    if (IsKeyDown(KEY_V))     vm->cpu.flags.keys[0xF] = true;
}

void init_display(void) {
    SetTraceLogLevel(LOG_NONE);
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "REmu - VM");

    // Load embedded Chicago font from memory at 32px base size
    retroFont = LoadFontFromMemory(".ttf", ChicagoFLF_ttf, ChicagoFLF_ttf_len, 32, 0, 0);
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

void render(VM* vm) {
    BeginDrawing();

    ClearBackground(APP_BG);

    // Title centered with shadow using retroFont
    const char* title = "REmu VM";
    int fontSize = 32;
    int titleWidth = MeasureTextEx(retroFont, title, fontSize, 1).x;
    int titleX = (WINDOW_WIDTH - titleWidth) / 2;
    int titleY = 12;

    DrawTextEx(retroFont, title, (Vector2){titleX + 2, titleY + 2}, fontSize, 1, (Color){30,30,30,200});
    DrawTextEx(retroFont, title, (Vector2){titleX, titleY}, fontSize, 1, DARKGRAY);

    int screen_w = SCREEN_WIDTH * SCALE;
    int screen_h = SCREEN_HEIGHT * SCALE;
    Rectangle bezel = { SCREEN_X - 8, SCREEN_Y - 8, screen_w + 16, screen_h + 16 };
    DrawRectangleRounded(bezel, 0.08f, 12, BEZEL);

    Rectangle screenBg = { SCREEN_X, SCREEN_Y, screen_w, screen_h };
    DrawRectangleRounded(screenBg, 0.08f, 12, SCREEN_BG);

    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            if (vm->vram[y * SCREEN_WIDTH + x]) {
                draw_sprite_glow(SCREEN_X + x * SCALE, SCREEN_Y + y * SCALE);
            }
        }
    }

    int pad_w = 4 * BUTTON_SIZE + 3 * BUTTON_GAP;
    int pad_x = (WINDOW_WIDTH - pad_w) / 2;
    int startY = KEYPAD_TOP;

    DrawRectangleRounded((Rectangle){pad_x - 6, startY - 6, pad_w + 12, 4 * BUTTON_SIZE + 3 * BUTTON_GAP + 12}, 0.05f, 10, KEYPAD_BG);

    for (int i = 0; i < 16; i++) {
        int row = i / 4;
        int col = i % 4;
        int x = pad_x + col * (BUTTON_SIZE + BUTTON_GAP);
        int y = startY + row * (BUTTON_SIZE + BUTTON_GAP);
        Color c = vm->cpu.flags.keys[i] ? DARKGRAY : BUTTON_BG;
        DrawRectangleRounded((Rectangle){x, y, BUTTON_SIZE, BUTTON_SIZE}, 0.15f, 6, c);
        DrawText(KEYMAP[i], x + BUTTON_SIZE / 3, y + BUTTON_SIZE / 3 - 2, 16, BLACK);
    }

    EndDrawing();
}
