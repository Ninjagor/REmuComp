#ifndef INTERPRETER_H
#define INTERPRETER_H


#include "vm/vm.h"
#include <stdint.h>

void interpret_op(VM* vm, uint16_t words[4]);

#endif
