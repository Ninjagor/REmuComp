# pragma  once

#include "vm/vm.h"


void poll_window_close(VM* vm);
void init_display(void);
void render(VM* vm);
void poll_keys(VM* vm);
