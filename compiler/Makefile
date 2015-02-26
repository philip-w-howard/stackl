COPTS=-Wall -g -c  -O0 -std=c++0x
OBJS=main.o \
	 scanner.o \
	 cSymbolTable.o \
	 cDeclNode.o \
	 cBinaryExprNode.o \
	 parser.o \
	 codegen.o \

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

.c.o:
	g++ $(COPTS) $? -o $@

main.o: main.cpp parser.c scanner.c 
	g++ $(COPTS) main.cpp -o $@

scanner.c: stackl.l parser.c
	flex -o scanner.c stackl.l

parser.c: stackl.y
	bison --defines=parser.h stackl.y -o parser.c

scanner.o: scanner.c 
	g++ $(COPTS) -Wno-sign-compare scanner.c -o $@

stacklc: $(OBJS)
	g++ $(OBJS) -o stacklc

