/* Copyright 2016 Lauri Gustafsson */
/* println.c: implements single null terminated line printing to stdio. */

#include "println.h"

/* Returns the numer of characters before \n +1 in str */
int countString (char* str) {
    int n;
    for (n=0; str[n]!='\n'; ++n);
    return n+1;
}

/* Calls write to stdout to display msg
   msg MUST BE null terminated !!! */
void println(char* msg) {
    int len = countString(msg);

    /* These are needed to avoid some relocation error... */
    char** pmsg = &msg;
    int* plen = &len;

    /* syscall write(int fd, const void *buf, size_t count) */
    asm (
    	"ldr %%r1, [%[msg]];"   /* buf = msg */
    	"ldr %%r2, [%[len]];"   /* count = len */
    	"mov %%r0, $1;"         /* file handle 1 is stdout */
    	"mov %%r7, $4;"         /* write is syscall 4 */
    	"swi $0;"               /* invoke syscall */
        : : [msg] "r" (pmsg), [len] "r" (plen) :  /* msg and len */
	);
}
