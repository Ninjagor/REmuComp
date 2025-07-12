#include <stdlib.h>
#include "memory/mem.h"
#include "utils/types.h"

Result initialize_memory(Memory* memory) {
  if (memory->initialized == true) {
    return ERROR;
  }

  void* temp = (void*)calloc(1, memory->size);

  if (temp == NULL) {
    return ERROR;
  }

  memory->memory = temp;
  memory->initialized = true;

  return SUCCESS;
}

