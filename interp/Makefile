COPTS=-Wall -g -c  -O0 -pthread
COPTS2=-Wall -g -c -O0 -Wno-deprecated-declarations -pthread
OBJS=stackl.o \
	 opcodes.o \
	 machine.o \
	 vmem.o \
	 loader.o \
	 io_handler.o \
	 dma_term.o \
	 pio_term.o \
	 disk.o \

all: stackl slasm

clean:
	rm -f $(OBJS)
	rm -f stackl
	rm -f makebin
	rm -f slasm
	rm -f slasm.o

.c.o:
	gcc $(COPTS) $? -o $@

stackl: $(OBJS)
	gcc $(OBJS) -o stackl -lpthread

slasm: slasm.o
	gcc slasm.o -o slasm

