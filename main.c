/* Copyright 2016 Lauri Gustafsson */
/* main.c: implements entry and exit for the program and demo setup */

#include "println.h"

void main() {
	println("Kalteri Mafia rul3z!!1\n");
}

void _start() {

    main();

    /* exit syscall */
    asm (
        "mov %r0, $0;"     /* status = 0 */
        "mov %r7, $1;"     /* exit is syscall 1 */
        "swi $0;"          /* invoke syscall */
    );
}
