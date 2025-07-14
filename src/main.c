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

static bool error_exit = false;

void handle_sigint(int sig) {
    if (error_exit) {
        printf("\nREmu VM Exited with an error.\n");
    } else {
        printf("\nREmu VM Exited successfully.\n");
    }

    printf("============================\n");
    exit(error_exit ? 1 : 0);
}

void set_error_exit_flag(bool val) {
    error_exit = val;
}

int main(int argc, char* argv[]) {
    // signal(SIGSEGV, segfault_handler);
    signal(SIGINT, handle_sigint);
    return cli_main(argc, argv);
}
