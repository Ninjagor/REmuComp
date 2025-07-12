#include "assembler/assembler.h"
#include "vm/vm.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

static void print_usage() {
    printf("Usage:\n");
    printf("  remu build [flags]           Build the assembly code\n");
    printf("    flags:\n");
    printf("      -q    Quiet mode (suppress output)\n");
    printf("      -v    Verbose mode (print binary output)\n\n");

    printf("  remu run <file> [flags]      Run the binary file\n");
    printf("    flags:\n");
    printf("      -d    Debug mode\n");
}

int cli_main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    if (strcmp(argv[1], "build") == 0) {
        if (argc < 3) {
            printf("Missing source file for build\n");
            print_usage();
            return 1;
        }

        const char* filepath = argv[2];
        bool quiet = false, verbose = false;

        for (int i = 3; i < argc; i++) {
            if (strcmp(argv[i], "-q") == 0) {
                quiet = true;
            } else if (strcmp(argv[i], "-v") == 0) {
                verbose = true;
            } else {
                printf("Unknown flag '%s' for build\n", argv[i]);
                return 1;
            }
        }

        int res = assemble(filepath);
        if (res != 0) {
            if (!quiet) printf("Build failed\n");
            return res;
        }
        if (!quiet) printf("Build succeeded\n");
        // If verbose, you can add code here to print the binary output.
        return 0;

    } else if (strcmp(argv[1], "run") == 0) {
        if (argc < 3) {
            printf("Missing file to run\n");
            print_usage();
            return 1;
        }

        const char* filepath = argv[2];
        bool debug = false;

        for (int i = 3; i < argc; i++) {
            if (strcmp(argv[i], "-d") == 0) {
                debug = true;
            } else {
                printf("Unknown flag '%s' for run\n", argv[i]);
                return 1;
            }
        }

      VM vm = {0};

      Result init = initialize_vm(&vm);
      if (init == ERROR) {
        return 1;
      }


      load_program(&vm, filepath);

      printf("\n============================\nSTDOUT\n\n");

      Result r = run_program(&vm);

      printf("============================\n");

      if (debug) {
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
      }

      return 0;

    } else {
        printf("Unknown command '%s'\n", argv[1]);
        print_usage();
        return 1;
    }
}
