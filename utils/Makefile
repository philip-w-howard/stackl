COPTS=-Wall -g -c -O0 

all: makebin copy2disk

clean:
	rm -f *.o
	rm -f makebin
	rm -f copy2disk

.c.o:
	gcc $(COPTS) $? -o $@

makebin: makebin.o makebin.c
	gcc makebin.o -o makebin

copy2disk: copy2disk.o copy2disk.c
	gcc copy2disk.o -o copy2disk

