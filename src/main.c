/* Copyright 2016 Lauri Gustafsson */
/* main.c: implements entry and exit for the program and demo setup */

#include "dnload.h"

#include "globals.h"

#include <SDL.h>
#include <stdio.h>
#include "synth.c"
#include "video.c"

#ifdef USE_LD
int main()
#else
void _start()
#endif
{
    dnload();
	dnload_puts("Kalteri Mafia rul3z while preparing the demo!!1");
    dnload_SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER);

    videoInit(G_VIDEO_X, G_VIDEO_Y);

    while (gDemoRunning) {
        
    }

    /* exit syscall */
#ifdef USE_LD
    return 0;
#else
    asm_exit();
#endif
}
