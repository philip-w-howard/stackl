COPTS=-Wall -g -c  -O0 -Wno-deprecated-declarations
OBJS=stackl.o \
	 opcodes.o \
	 machine.o \
	 syscall.o \
	 loader.o \
	 sched.o \

all: stackl

clean:
	rm -f $(OBJS)
	rm -f stackl

.c.o:
	gcc $(COPTS) $? -o $@

stackl: $(OBJS)
	gcc $(OBJS) -o stackl

