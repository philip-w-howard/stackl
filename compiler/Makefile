COPTS=-Wall -g -c  -O0 -std=c++0x
OBJS= scanner.o \
	 parser.o \
	 cSymbolTable.o \
	 cDecl.o \
	 cBinaryExpr.o \
	 codegen.o \
	 stacklc.o \

all: stacklc

clean:
	rm -f $(OBJS)
	rm -f scanner.c
	rm -f parser.c
	rm -f parser.h
	rm -f stacklc
	rm -f out

.cpp.o:
	g++ $(COPTS) $? -o $@

parser.o: parser.c
	g++ $(COPTS) $? -o $@ -DYYDEBUG=1

.c.o:
	g++ $(COPTS) $? -o $@

stacklc.o: stacklc.cpp parser.o scanner.o 
	g++ $(COPTS) stacklc.cpp -o $@

scanner.c: stackl.l parser.c
	flex -o scanner.c stackl.l

parser.c: stackl.y
	bison --defines=parser.h stackl.y -o parser.c

scanner.o: scanner.c 
	g++ $(COPTS) -Wno-sign-compare scanner.c -o $@

stacklc: $(OBJS)
	g++ $(OBJS) -o stacklc

