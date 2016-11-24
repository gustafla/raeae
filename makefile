# A makefile.

SRCS=$(wildcard *.c) $(wildcard */*.c)
TARGET=raeae
OBJS=$(SRCS:.c=.o)
CC=gcc
CFLAGS=-nostdlib -MMD -MP -Wl,--build-id=none
INCLUDES=-I./
LFLAGS=-L/opt/vc/lib
#LIBS=-lGLESv2 -lEGL -lbcm_host
LIBS=

default: $(TARGET)
	strip --strip-all $(TARGET)
	strip --remove-section .ARM.attributes $(TARGET)
	strip --remove-section .comment $(TARGET)
	@echo "Finished."
	
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS) $(LFLAGS) $(LIBS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	
clean:
	find ./ -type f -name "*.o" -delete
	find ./ -type f -name "*.d" -delete
	rm -f $(TARGET)

-include *.d
