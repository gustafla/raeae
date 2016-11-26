TARGET=raeae
ENTRY=main.c
DNLOAD=~/programs/dnload/dnload.py
PYTHON=python2.7

all:
	$(PYTHON) $(DNLOAD) -c $(ENTRY) -v -o $(TARGET)

clean:
	rm -f *.S *.o *.unprocessed *.stripped *.bin *.ld $(TARGET)
