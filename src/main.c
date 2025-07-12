#include <stdlib.h>
#include <stdio.h>
#include "include/main.h"
#include "assembler/assembler.h"
#include "constants.h"
#include "utils/types.h"
#include "vm/vm.h"

int main() {
  printf("\n=========================\n** REmuVM **\n=========================\n");


  Result a = assemble("/Users/rohit/rohit-project-work/remucomp/samples/sample3.rasm");
  if (a == ERROR) {
    printf("\n");
    return 1;
  }

  VM vm = {0};

  Result init = initialize_vm(&vm);
  if (init == ERROR) {
    return 1;
  }


  load_program(&vm, "/Users/rohit/rohit-project-work/remucomp/out/a.bin");

  printf("\n============================\nSTDOUT\n\n");

  Result r = run_program(&vm);


  printf("============================\n");

  printf("\nCPU Register Peek: \n");
  for (int i = 0; i < CPU_REGISTER_COUNT; i++) {
    char* p = "";
    if (i < 10) {
      p = "0";
    }
    printf("R%s%i 0x%04X ", p, i, vm.cpu.registers[i].value);
    if ((i+1)%4 == 0) {
      printf("\n");
    }
  }

  if (r == ERROR) {
    // :(
  }

  printf("\n");
  return 0;
}
