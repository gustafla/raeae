/* Copyright 2016 Lauri Gustafsson */
/* main.c: implements entry and exit for the program and demo setup */

#include "println.c"
#include <SDL.h>
#include "video.c"

void _start() {
    println("Kalteri Mafia rul3z while preparing the demo!!1\n");
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER);
    
    videoInit(640, 480);

    while (1);

    SDL_Quit();

    /* exit syscall */
    asm (
        "mov %r0, $0;"     /* status = 0 */
        "mov %r7, $1;"     /* exit is syscall 1 */
        "swi $0;"          /* invoke syscall */
    );
}
