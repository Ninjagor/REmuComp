#pragma once

/** MEMORY RELATED STUFF **/

#define RAM_SIZE 65536  // 64 KB total RAM

// 42 KB program segment
#define PROGRAM_START 0x0000
#define PROGRAM_SIZE  0xA800  // 48 KB = 0xC000 bytes
#define PROGRAM_END (PROGRAM_START + PROGRAM_SIZE - 1)  // 0xBFFF
#define PROGRAM_MAX PROGRAM_END

// 6 KB VRAM
#define VRAM_SIZE 0x1800
#define VRAM_START (PROGRAM_END + 1)
#define VRAM_END (VRAM_START + VRAM_SIZE - 1) 

// 1 KB stack
#define STACK_SIZE 0x400
#define STACK_START (VRAM_END + 1)
#define STACK_END (STACK_START + STACK_SIZE - 1)

// 1 KB Spritesheet
#define SPRITESHEET_SIZE 0x400
#define SPRITESHEET_START (STACK_END + 1) 
#define SPRITESHEET_END (SPRITESHEET_START + SPRITESHEET_SIZE - 1)

// 11 KB data segment
#define DATA_SEGMENT_SIZE 0x2C00
#define DATA_SEGMENT_START (SPRITESHEET_END + 1)
#define DATA_SEGMENT_END (DATA_SEGMENT_START + DATA_SEGMENT_SIZE - 1) 



/** CPU RELATED STUFF **/
#define CPU_REGISTER_COUNT 32
// const int CPU_REGISTER_COUNT = 15;


/** STRING RELATED CONSTANTS **/
#define MAX_STRINGS 2048
#define MAX_STRING_LENGTH 1024

/** GRAPHICS **/
#define SCREEN_WIDTH 96
#define SCREEN_HEIGHT 64
#define SPRITE_SIZE 8

/** TIMING **/
#define VM_OPS_PER_SECOND 2000

/** SPRITES **/
#define MAX_SPRITE_SIZE 8
#define MAX_SPRITES 1024
#define SPRITE_SIZE 8
