COPTS=-Wall -g -c  -O0 
OBJS=stackl.o \
	 opcodes.o \
	 machine.o \

all: stackl

clean:
	rm -f $(OBJS)
	rm -f stackl

.c.o:
	gcc $(COPTS) $? -o $@

stackl: $(OBJS)
	gcc $(OBJS) -o stackl

