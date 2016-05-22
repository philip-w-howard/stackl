COPTS=-Wall -g -c -O0 

all: copy2disk

clean:
	rm -f *.o
	rm -f copy2disk

.c.o:
	gcc $(COPTS) $? -o $@

copy2disk: copy2disk.o copy2disk.c
	gcc copy2disk.o -o copy2disk

