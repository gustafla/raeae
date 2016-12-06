# A makefile.

#SRCS=$(wildcard *.c) $(wildcard */*.c)
SRCS=main.c
TARGET=raeae
OBJS=$(SRCS:.c=.o)
CC=gcc
CFLAGS=-Wl,--build-id=none -march=armv6j -mfpu=vfp -mfloat-abi=hard -ffast-math -fno-inline -fpeephole2 -fexpensive-optimizations -nostdlib -fomit-frame-pointer -fno-exceptions -fno-asynchronous-unwind-tables
INCLUDES=-I./ -I/usr/include/SDL -I/opt/vc/include
LFLAGS=-L/opt/vc/lib -L/usr/lib -lGLESv2 -lEGL -lbcm_host -lSDL

all: $(TARGET)
	cp $(TARGET).bin $(TARGET).stripped
	sstrip $(TARGET).stripped
	cp $(TARGET).stripped $(TARGET).nounpacker
	xz --format=lzma --lzma1=preset=9,lc=1,lp=0,pb=0 $(TARGET).nounpacker
	printf "HOME=/tmp/i;sed 1d \$$0|lzcat>~;chmod +x ~;~;exit\n" >$(TARGET)
	cat $(TARGET).nounpacker.lzma >> $(TARGET)
	chmod +x $(TARGET)
	@echo "Finished."
	
$(TARGET):
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET).bin $(SRCS) $(LFLAGS)

#.c.o:
#	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	
clean:
	find ./ -type f -name "*.o" -delete
	find ./ -type f -name "*.d" -delete
	rm -f $(TARGET)*
	rm -f *.extra.cpp
