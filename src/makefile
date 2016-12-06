TARGET=raeae
ENTRY=main.c
DNLOAD=~/programs/dnload/dnload.py
PYTHON=python2.7
LFLAGS=-lSDL -lGLESv2 -lEGL -lbcm_host

all:
	$(PYTHON) $(DNLOAD) -a sdl1 -v -c $(ENTRY) -o $(TARGET)

clean:
	rm -f *.S *.o *.unprocessed *.stripped *.bin *.ld $(TARGET)*
