#include <string.h>
#include "assembler/assembler.h"

OpCode get_opcode_from_str(const char* instr) {
    if (strcmp(instr, "MOVI") == 0) return MOVI;
    if (strcmp(instr, "MOVR") == 0) return MOVR;
    if (strcmp(instr, "ADD") == 0) return ADD;
    if (strcmp(instr, "SUB") == 0) return SUB;
    if (strcmp(instr, "MUL") == 0) return MUL;
    if (strcmp(instr, "DIV") == 0) return DIV;
    if (strcmp(instr, "MOD") == 0) return MOD;
    if (strcmp(instr, "AND") == 0) return AND;
    if (strcmp(instr, "OR") == 0) return OR;
    if (strcmp(instr, "XOR") == 0) return XOR;
    if (strcmp(instr, "NOT") == 0) return NOT;
    if (strcmp(instr, "CMP") == 0) return CMP;
    if (strcmp(instr, "JE") == 0) return JE;
    if (strcmp(instr, "JNE") == 0) return JNE;
    if (strcmp(instr, "JL") == 0) return JL;
    if (strcmp(instr, "JLE") == 0) return JLE;
    if (strcmp(instr, "JG") == 0) return JG;
    if (strcmp(instr, "JGE") == 0) return JGE;
    if (strcmp(instr, "JMP") == 0) return JMP;
    if (strcmp(instr, "CALL") == 0) return CALL;
    if (strcmp(instr, "RET") == 0) return RET;
    if (strcmp(instr, "HALT") == 0) return HALT;
    if (strcmp(instr, "PUSH") == 0) return PUSH;
    if (strcmp(instr, "POP") == 0) return POP;
    if (strcmp(instr, "SHL") == 0) return SHL;
    if (strcmp(instr, "SHR") == 0) return SHR;
    if (strcmp(instr, "SAR") == 0) return SAR;
    if (strcmp(instr, "ROL") == 0) return ROL;
    if (strcmp(instr, "ROR") == 0) return ROR;
    if (strcmp(instr, "LOAD") == 0) return LOAD;
    if (strcmp(instr, "STORE") == 0) return STORE;
    if (strcmp(instr, "LOADR") == 0) return LOADR;
    if (strcmp(instr, "STORER") == 0) return STORER;
    if (strcmp(instr, "PRINT") == 0) return PRINT;
    if (strcmp(instr, "PUTC") == 0) return PUTC;
    if (strcmp(instr, "DRAW") == 0) return DRAW;
    if (strcmp(instr, "CLS") == 0) return CLS;

    if (strcmp(instr, "ALLOWMOD") == 0) return ENABLESMOD;
    if (strcmp(instr, "DISABLEMOD") == 0) return DISABLESMOD;

    // Unknown instruction
    return -1;
}
