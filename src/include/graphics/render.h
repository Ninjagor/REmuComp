# pragma  once

#include "vm/vm.h"


void poll_window_close(VM* vm);
void init_display(void);
void render(VM* vm);
void poll_keys(VM* vm);
void init_audio(void);
void start_beep(void);
void stop_beep(void);
void cleanup_audio(void);
