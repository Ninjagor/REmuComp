#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    char*** lines;
    size_t count;
    size_t capacity;
} ParsedLines;


uint8_t* parse_hex_string(const char* input, size_t* out_len);

char** split_lines(const char* raw_code, size_t* out_count);

char** parse_line(const char* line);

void free_parsed_lines(ParsedLines* plines);

ParsedLines parse_all_lines(char** lines, size_t count);

#endif
