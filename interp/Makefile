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

BINOBJS=makebin.o \
	 vmem.o \
	 loader.o \
	 machine.o \

all: stackl makebin

clean:
	rm -f $(OBJS)
	rm -f $(BINOBJS)
	rm -f stackl
	rm -f makebin

.c.o:
	gcc $(COPTS) $? -o $@

stackl: $(OBJS)
	gcc $(OBJS) -o stackl -lpthread

makebin: $(BINOBJS)
	gcc $(BINOBJS) -o makebin -lpthread

