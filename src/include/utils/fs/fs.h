#ifndef FS_H
#define FS_H

#include <stddef.h>
#include <stdint.h>

char* read_file_to_string(const char* filepath);
uint8_t* read_file_to_buffer(const char* filepath, size_t* out_size);

#endif
