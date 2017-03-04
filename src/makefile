TARGET=raeae
ENTRY=main.c
PYTHON=python2.7
DNLOAD=dnload/dnload.py

debug:CFLAGS=-g -fwhole-program -I/usr/include/SDL -I/opt/vc/include -DUSE_LD -DDNLOAD_VIDEOCORE
debug:LFLAGS=-L/opt/vc/lib -lc -lSDL -lGLESv2 -lEGL -lbcm_host

debugnovc:CFLAGS=-g -fwhole-program -I/usr/include/SDL2 -DUSE_LD
debugnovc:LFLAGS=-lc -lSDL2 -lGL

all:
	make debug && make size

size:
	$(PYTHON) -m $(DNLOAD) -a sdl1 -v -c $(ENTRY) -o $(TARGET)

debug:
	$(PYTHON) -m $(DNLOAD) -a sdl1 -v $(ENTRY)
	g++ $(CFLAGS) $(ENTRY) $(LFLAGS) -o $(TARGET).debug 

debugnovc:
	$(PYTHON) $(DNLOAD) -a sdl2 -v $(ENTRY)
	g++ $(CFLAGS) $(ENTRY) $(LFLAGS) -o $(TARGET).debugnovc

novc:
	$(PYTHON) $(DNLOAD) -a sdl2 -c $(ENTRY) -v -o $(TARGET).novc

clean:
	rm -f *.S *.o *.unprocessed *.stripped *.bin *.ld $(TARGET)*
