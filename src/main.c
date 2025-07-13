#include <stdlib.h>
#include <stdio.h>
#include "include/main.h"
#include "assembler/assembler.h"
#include "constants.h"
#include "utils/types.h"
#include "vm/vm.h"
#include <signal.h>
#include <unistd.h>
#include "cli.h"

void segfault_handler(int sig) {
    (void)sig;
    fprintf(stderr, "\nREmuVM - Unsafe operation detected. Program exited.\n");
    _exit(1);
}

int main(int argc, char* argv[]) {
    // signal(SIGSEGV, segfault_handler);
    return cli_main(argc, argv);
}
