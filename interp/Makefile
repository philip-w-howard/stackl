COPTS=-Wall -g -c -O0 -Wno-deprecated-declarations -pthread
OBJS=stackl.o \
	 opcodes.o \
	 machine.o \
	 vmem.o \
	 loader.o \
	 io_handler.o \
	 dma_term.o \
	 pio_term.o \
	 disk.o \

all: stackl

clean:
	rm -f $(OBJS)
	rm -f stackl

.c.o:
	gcc $(COPTS) $? -o $@

stackl: $(OBJS)
	gcc $(OBJS) -o stackl -lpthread

