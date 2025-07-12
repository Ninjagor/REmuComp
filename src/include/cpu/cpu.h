#ifndef CPU_H
#define CPU_H

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

typedef enum ModificationFlag {
  NOTALLOWED = 0x0000,
  ALLOWED = 0x0001
} ModificationFlag;

typedef struct Flags {
  CmpFlag cmp_flag;
  InterruptFlag program_interrupt;
  ModificationFlag modification;
} Flags;

typedef struct CPU {
  size_t pc;
  Register registers[CPU_REGISTER_COUNT];
  Flags flags;
} CPU;



#endif
