#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "utils/types.h"

typedef enum OpCode {
  // Arithmetic and Logic
  MOVI   = 0x01,  // MOV Rdst, #imm
  MOVR   = 0x02,  // MOV Rdst, Rsrc
  ADD       = 0x03,
  SUB       = 0x04,
  MUL       = 0x05,
  DIV       = 0x06,
  MOD       = 0x07,
  AND       = 0x08,
  OR        = 0x09,
  XOR       = 0x0A,
  NOT       = 0x0B,

  // Comparison and Conditional
  CMP       = 0x10,
  JE        = 0x11,
  JNE       = 0x12,
  JL        = 0x13,
  JLE       = 0x14,
  JG        = 0x15,
  JGE       = 0x16,

  // Control Flow
  JMP       = 0x20,
  CALL      = 0x21,
  RET       = 0x22,
  HALT      = 0x23,

  // Stack
  PUSH      = 0x30,
  POP       = 0x31,

  // Shift and Rotate
  SHL       = 0x40,
  SHR       = 0x41,
  SAR       = 0x42,
  ROL       = 0x43,
  ROR       = 0x44,

  // Memory Access
  LOAD      = 0x50,
  STORE     = 0x51,
  LOADR     = 0x52,
  STORER    = 0x53,

  // Text
  PRINT     = 0x60,
  PUTC      = 0x61,

  // Graphics
  DRAW      = 0x70,
  CLS       = 0x71,

  // MEMORY
  CLSM = 0x81,

  // FLAGS
  ENABLESMOD = 0x90,
  DISABLESMOD = 0x91
} OpCode;

Result assemble(const char* filepath);

#endif
