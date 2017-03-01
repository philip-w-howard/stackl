VPATH:=dbg
COPTS=-Wall -g -c -O0 -pthread
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
	 struct_decl.o \
	 variable.o \
	 asm_list.o \
	 abstract_syntax_tree.o \
	 function.o \
	 string_utils.o \
	 stackl_debugger.o \
	 debugger_interface.o \
	 debugger_command.o

all: stackl slasm

clean:
	rm -f $(OBJS)
	rm -f stackl
	rm -f makebin
	rm -f slasm
	rm -f slasm.o
	rm -f rapidxml.o
	rm -f rapidxml_utils.o

opcodes.o: opcodes.c opcode_defs.h

.c.o: 
	gcc $(COPTS) $< -o $@

.cpp.o:
	g++ $(COPTS) $< -o $@ -std=c++1y

opcode_defs.h: slasm
	./slasm -defs

stackl: $(OBJS)
	gcc $(OBJS) -o stackl -lpthread -lstdc++ -lm

slasm: slasm.c formatstr.o
	gcc -g -O0 slasm.c -o slasm formatstr.o

