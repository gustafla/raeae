/* Returns the numer of characters before \n +1 in str */
int countString (char* str) {
    int n;
    for (n=0; str[n]!='\n'; ++n);
    return n+1;
}

/* Calls write to stdout to display msg
   msg MUST BE null terminated !!! */
void print(char* msg) {
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

void main() {
	print("Kalteri Mafia rul3z!!1\n");
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
