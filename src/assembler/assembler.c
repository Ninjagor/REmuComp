#include "assembler/assembler.h"
#include "assembler/opmeta.h"
#include "assembler/labelmap.h"
#include "assembler/opcode.h"
#include "utils/fs/fs.h"
#include "utils/types.h"
#include "utils/structs/dynbuf.h"
#include "parser/parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

static void init_string_table(StringTable* st) {
    st->strings = NULL;
    st->count = 0;
    st->capacity = 0;
}

static int add_string(StringTable* st, const char* str) {
    for (size_t i = 0; i < st->count; i++) {
        if (strcmp(st->strings[i], str) == 0) {
            return (int)i;
        }
    }
    if (st->count == st->capacity) {
        size_t new_cap = st->capacity ? st->capacity * 2 : 8;
        char** tmp = realloc(st->strings, new_cap * sizeof(char*));
        if (!tmp) return -1;
        st->strings = tmp;
        st->capacity = new_cap;
    }
    st->strings[st->count] = strdup(str);
    if (!st->strings[st->count]) return -1;
    return (int)(st->count++);
}

static void free_string_table(StringTable* st) {
    for (size_t i = 0; i < st->count; i++) {
        free(st->strings[i]);
    }
    free(st->strings);
    st->strings = NULL;
    st->count = 0;
    st->capacity = 0;
}

static void write_string_table(FILE* f, StringTable* st) {
    for (size_t i = 0; i < st->count; i++) {
        const char* s = st->strings[i];
        size_t len = strlen(s);
        for (size_t j = 0; j <= len; j++) { 
            uint16_t w = (uint16_t)(uint8_t)s[j];
            fwrite(&w, sizeof(uint16_t), 1, f);
        }
    }
}

static Result first_pass(LabelMap* labels, ParsedLines* plines) {
  uint16_t pc = 0;

  for (size_t i = 0; i < plines->count; i++) {
    char** toks = plines->lines[i];
    if (!toks[0]) continue;

    if (toks[0][0] == '_') {
      if (!add_label(labels, toks[0], pc)) {
        printf("Linking label %s failed\n", toks[0]);
        return ERROR;
      }
      continue;
    }

    if ((int)get_opcode_from_str(toks[0]) == -1) {
      printf("\nCOMPILATION ERROR: Invalid Operation %s", toks[0]);
      return ERROR;
    }

    pc += 8;
  }
  return SUCCESS;
}

static Result second_pass(DynBuffer* opcodes, LabelMap* labels, ParsedLines* plines, StringTable* st) {
  for (size_t i = 0; i < plines->count; i++) {
    char** toks = plines->lines[i];
    if (!toks[0]) continue;
    if (toks[0][0] == '_') continue;

    int opcode = get_opcode_from_str(toks[0]);

    if (opcode == -3) {
      if (!toks[1] || !toks[2] || !toks[3]) {
        printf("COMPILATION ERROR: DSTR requires register, string literal, and address\n");
        return ERROR;
      }
      int regnum = -1;
      if (toks[1][0] == 'R') regnum = atoi(toks[1] + 1);
      if (regnum < 0) {
        printf("COMPILATION ERROR: DTRS requires a valid register\n");
        return ERROR;
      }

      const char* str_literal = toks[2];
      size_t len = strlen(str_literal);
      if (len < 2 || str_literal[0] != '"' || str_literal[len - 1] != '"') {
        printf("COMPILATION ERROR: DTRS string literal must be quoted\n");
        return ERROR;
      }
      char* string_content = strndup(str_literal + 1, len - 2);
      if (!string_content) {
        printf("COMPILATION ERROR: Memory allocation failed\n");
        return ERROR;
      }

      uint16_t start_addr = (uint16_t)strtol(toks[3], NULL, 0);

      for (size_t idx = 0; idx <= strlen(string_content); idx++) { // include null terminator
        uint8_t byte = string_content[idx];

        appendWord(opcodes, MOVI);
        appendWord(opcodes, (uint16_t)regnum);
        appendWord(opcodes, (uint16_t)byte);
        appendWord(opcodes, 0x0000);

        appendWord(opcodes, STORE);
        appendWord(opcodes, (uint16_t)regnum);
        appendWord(opcodes, start_addr + (uint16_t)idx);
        appendWord(opcodes, 0x0000);
      }

      free(string_content);
      continue;
    }

    if (opcode == -2) {
      if (!toks[1] || !toks[2]) {
        printf("COMPILATION ERROR: STRS requires register and string literal\n");
        return ERROR;
      }

      int regnum = -1;
      if (toks[1][0] == 'R') regnum = atoi(toks[1] + 1);
      if (regnum < 0) {
        printf("COMPILATION ERROR: STRS requires valid register\n");
        return ERROR;
      }

      const char* str_literal = toks[2];
      size_t len = strlen(str_literal);
      if (len < 2 || str_literal[0] != '"' || str_literal[len - 1] != '"') {
        printf("COMPILATION ERROR: STRS string literal must be quoted\n");
        return ERROR;
      }

      char* string_content = strndup(str_literal + 1, len - 2);
      if (!string_content) {
        printf("COMPILATION ERROR: Memory allocation failed\n");
        return ERROR;
      }

      int idx = add_string(st, string_content);
      free(string_content);
      if (idx < 0) {
        printf("COMPILATION ERROR: Failed to add string to table\n");
        return ERROR;
      }

      uint16_t dest_addr = 0x0000;
      if (toks[3]) {
        // parse the 3rd operand (dest memory address)
        dest_addr = (uint16_t)strtol(toks[3], NULL, 0);
      }

      appendWord(opcodes, 0x80);
      appendWord(opcodes, (uint16_t)regnum);
      appendWord(opcodes, (uint16_t)idx);
      appendWord(opcodes, dest_addr);

      continue;
    }

    if (opcode == -1) {
      printf("\nCOMPILATION ERROR: Invalid Operation %s", toks[0]);
      return ERROR;
    }

    int operand_count = get_operand_count((OpCode)opcode);
    int actual_operands = 0;
    for (int j = 1; j < 4; j++) {
      if (toks[j]) actual_operands++;
    }

    if (actual_operands < operand_count) {
      printf("\nCOMPILATION ERROR: Missing operands for %s\n", toks[0]);
      return ERROR;
    }
    if (actual_operands > operand_count) {
      printf("\nCOMPILATION ERROR: Too many operands for %s\n", toks[0]);
      return ERROR;
    }

    appendWord(opcodes, (uint16_t)opcode);

    if (opcode == JMP || opcode == CALL || opcode == JE || opcode == JNE || opcode == JL ||
        opcode == JLE || opcode == JG || opcode == JGE) {
      const char* label = toks[1];
      int target_addr = get_label_address(labels, label);
      if (target_addr == -1) {
        printf("COMPILATION ERROR: Undefined label %s\n", label);
        return ERROR;
      }
      appendWord(opcodes, (uint16_t)target_addr);
      appendWord(opcodes, 0x0000);
      appendWord(opcodes, 0x0000);
      continue;
    }

    if (opcode == STORE || opcode == LOAD || opcode == LOADR) {
      int regnum = (toks[1][0] == 'R') ? atoi(toks[1] + 1) : -1;
      int addr = (int)strtol(toks[2], NULL, 0);
      if (regnum < 0) {
        printf("COMPILATION ERROR: %s requires register as first operand\n", toks[0]);
        return ERROR;
      }
      appendWord(opcodes, (uint16_t)regnum);
      appendWord(opcodes, (uint16_t)addr);
      appendWord(opcodes, 0x0000);
      continue;
    }

    int j = 1;
    for (; j <= operand_count; j++) {
      if (!toks[j]) {
        appendWord(opcodes, 0x0000);
      } else {
        if (toks[j][0] == 'R') {
          int regnum = atoi(toks[j] + 1);
          appendWord(opcodes, (uint16_t)regnum);
        } else if (toks[j][0] == '#') {
          int imm = (int)strtol(toks[j] + 1, NULL, 0);
          appendWord(opcodes, (uint16_t)imm);
        } else {
          int imm = (int)strtol(toks[j], NULL, 0);
          appendWord(opcodes, (uint16_t)imm);
        }
      }
    }

    for (; j <= 3; j++) {
      appendWord(opcodes, 0x0000);
    }
  }
  return SUCCESS;
}

Result assemble(const char* filepath) {
  char* raw_code = read_file_to_string(filepath);
  if (!raw_code) return ERROR;

  const char* preamble = "JMP _START\n";
  size_t total_len = strlen(preamble) + strlen(raw_code) + 1;
  char* modified_code = malloc(total_len);
  if (!modified_code) {
    free(raw_code);
    return ERROR;
  }

  strcpy(modified_code, preamble);
  strcat(modified_code, raw_code);

  size_t line_count = 0;
  char** lines = split_lines(modified_code, &line_count);
  if (!lines) {
    free(modified_code);
    free(raw_code);
    return ERROR;
  }

  ParsedLines plines = parse_all_lines(lines, line_count);

  LabelMap labels;
  init_label_map(&labels);
  DynBuffer opcodes;
  initBuffer(&opcodes, 16);

  StringTable strtable;
  init_string_table(&strtable);

  Result res = first_pass(&labels, &plines);
  if (res == SUCCESS) res = second_pass(&opcodes, &labels, &plines, &strtable);

  free_label_map(&labels);
  free_parsed_lines(&plines);
  for (size_t i = 0; i < line_count; i++) free(lines[i]);
  free(lines);
  free(modified_code);
  free(raw_code);

  if (res == ERROR) {
    freeBuffer(&opcodes);
    free_string_table(&strtable);
    return ERROR;
  }

  FILE *f = fopen("out/a.bin", "wb");
  if (!f) {
    printf("\nINVALID FILE out/a.bin\n");
    freeBuffer(&opcodes);
    free_string_table(&strtable);
    return ERROR;
  }

  for (size_t i = 0; i < opcodes.size; i++) {
      if (i % 4 == 0) {
          printf("\n"); // new instruction
      }
      printf("0x%04X ", opcodes.data[i]);
  }
  printf("\n\nCompiled: %zu Bytes\n", opcodes.size);
  printf("\n");

  fwrite(opcodes.data, sizeof(uint16_t), opcodes.size, f);

  uint16_t delimiter = 0xFFFF;
  fwrite(&delimiter, sizeof(uint16_t), 1, f);

  write_string_table(f, &strtable);

  fclose(f);

  freeBuffer(&opcodes);
  free_string_table(&strtable);

  return SUCCESS;
}
