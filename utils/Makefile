COPTS=-Wall -g -c -O0 

all: copy2disk makedisk

clean:
	rm -f *.o
	rm -f copy2disk
	rm -f makedisk

.c.o:
	gcc $(COPTS) $? -o $@

copy2disk: copy2disk.o copy2disk.c
	gcc copy2disk.o -o copy2disk

makedisk: makedisk.o makedisk.c
	gcc makedisk.o -o makedisk

