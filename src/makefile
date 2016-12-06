TARGET=raeae
ENTRY=main.c
DNLOAD=~/programs/dnload/dnload.py
PYTHON=python2.7
CFLAGS=-g -fwhole-program -I/usr/include/SDL -I/opt/vc/include -DUSE_LD
LFLAGS=-lSDL -lGLESv2 -lEGL -lbcm_host

all:
	make size
	make debug

size:
	$(PYTHON) $(DNLOAD) -a sdl1 -v -c $(ENTRY) -o $(TARGET)

debug:
	$(PYTHON) $(DNLOAD) -a sdl1 -v $(ENTRY)
	gcc $(CFLAGS) main.c $(LFLAGS) -o $(TARGET).debug 

clean:
	rm -f *.S *.o *.unprocessed *.stripped *.bin *.ld $(TARGET)*
