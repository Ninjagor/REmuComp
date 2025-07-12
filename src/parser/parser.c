#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "parser/parser.h"

uint8_t* parse_hex_string(const char* input, size_t* out_len) {
    if (!input || !out_len) return NULL;

    size_t count = 0;
    const char* ptr = input;
    while (*ptr) {
        if (*ptr == ' ') count++;
        ptr++;
    }
    count++; 

    uint8_t* result = malloc(count);
    if (!result) return NULL;

    size_t idx = 0;
    char* copy = strdup(input);
    char* token = strtok(copy, " ");
    while (token) {
        result[idx++] = (uint8_t)strtol(token, NULL, 0);
        token = strtok(NULL, " ");
    }

    free(copy);
    *out_len = idx;
    return result;
}

void trim(char* str) {
    char* p = str;
    while (isspace((unsigned char)*p)) p++;
    if (p != str) memmove(str, p, strlen(p)+1);

    size_t len = strlen(str);
    while (len > 0 && isspace((unsigned char)str[len-1])) {
        str[len-1] = '\0';
        len--;
    }
}

void remove_commas(char* str) {
    char* src = str;
    char* dst = str;
    while (*src) {
        if (*src != ',') *dst++ = *src;
        src++;
    }
    *dst = '\0';
}

char** parse_line(const char* line) {
    char* copy = strdup(line);
    if (!copy) return NULL;

    char* comment = strchr(copy, ';');
    if (comment) *comment = '\0';

    trim(copy);
    if (copy[0] == '\0') {
        free(copy);
        return NULL;
    }
    if (copy[0] == ';') {
        free(copy);
        return NULL;
    }

    remove_commas(copy);

    char** tokens = calloc(4, sizeof(char*));
    if (!tokens) {
        free(copy);
        return NULL;
    }

    size_t idx = 0;
    char* tok = strtok(copy, " \t");
    while (tok && idx < 4) {
        tokens[idx++] = strdup(tok);
        tok = strtok(NULL, " \t");
    }

    free(copy);
    return tokens;
}

void append_parsed_line(ParsedLines* plines, char** parsed_line) {
    if (plines->count == plines->capacity) {
        size_t new_cap = plines->capacity ? plines->capacity * 2 : 16;
        char*** tmp = realloc(plines->lines, new_cap * sizeof(char**));
        if (!tmp) return;
        plines->lines = tmp;
        plines->capacity = new_cap;
    }
    plines->lines[plines->count++] = parsed_line;
}

void free_parsed_lines(ParsedLines* plines) {
    for (size_t i = 0; i < plines->count; i++) {
        char** line = plines->lines[i];
        if (line) {
            for (int j = 0; j < 4; j++) {
                free(line[j]);
            }
            free(line);
        }
    }
    free(plines->lines);
    plines->lines = NULL;
    plines->count = 0;
    plines->capacity = 0;
}

ParsedLines parse_all_lines(char** lines, size_t count) {
    ParsedLines plines = {0};

    for (size_t i = 0; i < count; i++) {
        char** parsed = parse_line(lines[i]);
        if (parsed) {
            append_parsed_line(&plines, parsed);
        }
    }
    return plines;
}

// caller must free
char** split_lines(const char* raw_code, size_t* out_count) {
    if (!raw_code) return NULL;

    size_t capacity = 16;
    size_t count = 0;
    char** lines = malloc(sizeof(char*) * capacity);
    if (!lines) return NULL;

    const char* start = raw_code;
    const char* pos = raw_code;

    while (*pos) {
        if (*pos == '\n') {
            size_t len = pos - start;
            char* line = malloc(len + 1);
            if (!line) goto cleanup;

            memcpy(line, start, len);
            line[len] = '\0';

            if (count == capacity) {
                capacity *= 2;
                char** tmp = realloc(lines, sizeof(char*) * capacity);
                if (!tmp) {
                    free(line);
                    goto cleanup;
                }
                lines = tmp;
            }

            lines[count++] = line;
            start = pos + 1;
        }
        pos++;
    }

    if (pos != start) {
        size_t len = pos - start;
        char* line = malloc(len + 1);
        if (!line) goto cleanup;

        memcpy(line, start, len);
        line[len] = '\0';

        if (count == capacity) {
            capacity *= 2;
            char** tmp = realloc(lines, sizeof(char*) * capacity);
            if (!tmp) {
                free(line);
                goto cleanup;
            }
            lines = tmp;
        }
        lines[count++] = line;
    }

    if (count == capacity) {
        char** tmp = realloc(lines, sizeof(char*) * (capacity + 1));
        if (!tmp) goto cleanup;
        lines = tmp;
    }
    lines[count] = NULL;

    if (out_count) *out_count = count;
    return lines;

cleanup:
    for (size_t i = 0; i < count; i++) free(lines[i]);
    free(lines);
    return NULL;
}
