CFLAGS=-Wall -g -c  -O0 -std=c++11
CPPFLAGS=-Wall -g -c  -O0 -std=c++11
OBJS= stacklopt.o \
	optimizations.o \

all: stacklopt

clean:
	rm -f $(OBJS)
	rm -f stacklopt
	rm -f out

.cpp.o:
	g++ $(CFLAGS) $? -o $@

.c.o:
	g++ $(CFLAGS) $? -o $@

stacklopt: $(OBJS)
	g++ $(OBJS) -o stacklopt

