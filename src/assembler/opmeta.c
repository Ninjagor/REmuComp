#include "assembler/opmeta.h"
#include "assembler/assembler.h"

int get_operand_count(OpCode op) {
  switch (op) {
    case MOVI: return 2;
    case MOVR: return 2;
    case ADD:
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
    case CLS: return 0;
    case CMP: return 2;
    case LOAD:
    case STORE:
    case LOADR:
    case CLSM:
    case LOADB: return 2;
    case DRAW:
    case INITDISPLAY: return 0;
    case STRB: return 3;
    case CEQ:
    case CG:
    case CGE:
    case CL:
    case CLE:
    case CNE: return 1;
    default: return -1;
  }
}
