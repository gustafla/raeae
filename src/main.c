/* Copyright 2016 Lauri Gustafsson */
/* main.c: implements entry and exit for the program and demo setup */

#include "dnload.h"

#ifdef USE_LD
int main()
#else
void _start()
#endif
{
    dnload();
	dnload_puts("Kalteri Mafia rul3z while preparing the demo!!1");
    dnload_SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER);

    while (1);

    /* exit syscall */
#ifdef USE_LD
    return 0;
#else
    asm_exit();
#endif
}
