TARGET=raeae
ENTRY=main.c
DNLOAD=~/programs/dnload/dnload.py
PYTHON=python2.7

debug:CFLAGS=-g -fwhole-program -I/usr/include/SDL -I/opt/vc/include -DUSE_LD -DDNLOAD_VIDEOCORE
debug:LFLAGS=-L/opt/vc/lib -lc -lSDL -lGLESv2 -lEGL -lbcm_host

debugnovc:CFLAGS=-g -fwhole-program -I/usr/include/SDL2 -DUSE_LD
debugnovc:LFLAGS=-lc -lSDL2 -lGLESv2

all:
	make debug && make size

size:
	$(PYTHON) $(DNLOAD) -a sdl1 -v -c $(ENTRY) -o $(TARGET)

debug:
	$(PYTHON) $(DNLOAD) -a sdl1 -v $(ENTRY)
	g++ $(CFLAGS) $(ENTRY) $(LFLAGS) -o $(TARGET).debug 

debugnovc:
	$(PYTHON) $(DNLOAD) -a sdl2 -v $(ENTRY)
	g++ $(CFLAGS) $(ENTRY) $(LFLAGS) -o $(TARGET).debugnovc

clean:
	rm -f *.S *.o *.unprocessed *.stripped *.bin *.ld $(TARGET)*
