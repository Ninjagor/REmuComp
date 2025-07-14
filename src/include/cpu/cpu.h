#ifndef CPU_H
#define CPU_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "constants.h"
#include "memory/mem.h"

typedef struct Register {
  int register_id;
  uint16_t value;
} Register;

typedef enum CmpFlag {
  EQUAL = 0x11,
  LESS = 0x22,
  GREATER = 0x33
} CmpFlag;

typedef enum InterruptFlag {
  RUNNING = 0x00,
  SFINISH = 0x11,
  EFINISH = 0x22
} InterruptFlag;

typedef enum GraphicsInitialized {
  INITIALIZED = 0x0001,
  AWAITING_INITIALIZATION = 0x0002,
  NOTINITIALIZED = 0x0000
} GraphicsInitialized;

typedef enum ModificationFlag {
  NOTALLOWED = 0x0000,
  ALLOWED = 0x0001
} ModificationFlag;

typedef struct Flags {
  CmpFlag cmp_flag;
  InterruptFlag program_interrupt;
  ModificationFlag modification;
  GraphicsInitialized graphics_initialized;
  int draw_flag; // 0-1
  int clear_flag; // 0-1
  int input_poll_flag; // 0-1

  bool keys[16];
  int delay_timer;
  int sound_timer;
} Flags;

typedef struct CPU {
  size_t pc;
  Register registers[CPU_REGISTER_COUNT];
  Flags flags;
} CPU;



#endif
