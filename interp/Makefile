VPATH:=dbg
CFLAGS=-Wall -g -c -O0 -pthread

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
	 gen_io.o \
	 formatstr.o \
	 struct_decl.o \
	 variable.o \
	 asm_list.o \
	 abstract_syntax_tree.o \
	 function.o \
	 string_utils.o \
	 debugger_interface.o \
	 debugger_command.o \
	 safe_read.o \

SOURCES=disk.c \
	dma_term.c \
	formatstr.c \
	gen_io.c \
	io_handler.c \
	loader.c \
	machine.c \
	opcodes.c \
	pio_term.c \
	slasm.c \
	stackl.c \
	timer.c \
	vmem.c \

.PHONY: all clean depend

all: depend stackl slasm stackl_norl

clean:
	rm -f $(OBJS)
	rm -f depend.mk
	rm -f stackl
	rm -f stackl_norl
	rm -f makebin
	rm -f slasm
	rm -f slasm.o
	rm -f rapidxml.o
	rm -f rapidxml_utils.o
	rm -f stackl_debugger.o
	rm -f stackl_debugger_norl.o

opcodes.o: opcodes.c opcode_defs.h

stackl_debugger.o : stackl_debugger.cpp
	g++ $(CFLAGS) $< -o $@ -std=c++11 
# in readline is installed, you can do the following to enable command line editing
#	g++ $(CFLAGS) $< -o $@ -std=c++11 -DLREADLINE

stackl_debugger_norl.o : stackl_debugger.cpp
	g++ $(CFLAGS) $< -o $@ -std=c++11

include depend.mk

depend: depend.mk

depend.mk: $(SOURCES)
	gcc -MM $(SOURCES) > depend.mk

.cpp.o:
	g++ $(CFLAGS) $< -o $@ -std=c++11

opcode_defs.h: slasm
	./slasm -defs

stackl: $(OBJS) stackl_debugger.o
	gcc $(OBJS) stackl_debugger.o -o stackl -lpthread -lstdc++ -lm 
# in readline is installed, you can do the following to enable command line editing
#	gcc $(OBJS) stackl_debugger.o -o stackl -lpthread -lstdc++ -lm -lreadline

stackl_norl: $(OBJS) stackl_debugger_norl.o 
	gcc $(OBJS) stackl_debugger_norl.o -o stackl_norl -lpthread -lstdc++ -lm

slasm: slasm.c formatstr.o
	gcc -g -O0 slasm.c -o slasm formatstr.o

