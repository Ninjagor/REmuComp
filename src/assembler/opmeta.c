#include "assembler/opmeta.h"
#include "assembler/assembler.h"

int get_operand_count(OpCode op) {
  switch (op) {
    case MOVI: return 2;
    case MOVR: return 2;
    case ADD:
    case MOD:
    case SUB:
    case AND:
    case MUL:
    case OR:
    case XOR: return 3;
    case NOT: return 2;
    case SHL:
    case SHR:
    case SAR:
    case ROL:
    case ROR: return 3;
    case PUSH:
    case POP: return 1;
    case PRINT: return 2;
    case PUTC: return 1;
    case JMP:
    case JE:
    case JNE:
    case JL:
    case JLE:
    case JG:
    case JGE:
    case CALL: return 1;
    case ENABLESMOD:
    case DISABLESMOD:
    case RET:
    case HALT:
    case POLL:
    case CLS: return 0;
    case CMP: return 2;
    case LOAD:
    case STORE:
    case LOADR:
    case CLSM:
    case RDI:
    case LOADB: return 2;
    case DRAW:
    case INITDISPLAY: return 0;
    case SPRB:
    case STRB: return 3;
    case CEQ:
    case CG:
    case CGE:
    case CL:
    case CLE:
    case STD:
    case STDI:
    case RTD:
    case STS:
    case STSI:
    case CNE: return 1;
    default: return -1;
  }
}
