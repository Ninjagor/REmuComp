#pragma once

/** MEMORY RELATED STUFF **/

#define RAM_SIZE 16384

// max 8 KB program
#define PROGRAM_START 0x0000
#define PROGRAM_MAX 0x1FFF

// 4 KB vram, 64 x 64 output
#define VRAM_SIZE 4096
#define VRAM_START 0x2000

// 1 KB stack
#define STACK_SIZE 1024
#define STACK_START (VRAM_START + VRAM_SIZE)

// 2 KB Spritesheet
#define SPRITESHEET_SIZE 2048
#define SPRITESHEET_START (STACK_START + STACK_SIZE)

// 0.5 KB data/variable segment
#define DATA_SEGMENT_SIZE 512
#define DATA_SEGMENT_START (SPRITESHEET_START + SPRITESHEET_SIZE)

// 0.5 KB Free - TODO


/** CPU RELATED STUFF **/
#define CPU_REGISTER_COUNT 16
// const int CPU_REGISTER_COUNT = 15;
