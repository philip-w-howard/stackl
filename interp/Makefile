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
	 timer.o \
	 formatstr.o \

all: stackl slasm

clean:
	rm -f $(OBJS)
	rm -f stackl
	rm -f makebin
	rm -f slasm
	rm -f slasm.o

opcodes.o: opcodes.c opcode_defs.h

.c.o: 
	gcc $(COPTS) $< -o $@

opcode_defs.h: slasm
	./slasm -defs

stackl: $(OBJS)
	gcc $(OBJS) -o stackl -lpthread

slasm: slasm.c formatstr.o
	gcc -g -O0 slasm.c -o slasm formatstr.o

