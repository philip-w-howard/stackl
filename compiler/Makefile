#########################################################
# Makefile for the stacklc compiler
#
# Note: there is a dependency on ../interp/opcode_def.h that isn't explicitly
#       stated in this makefile. The interpreter must be built before
#       the compiler

CFLAGS=-Wall -g -c  -O0 -std=c++11
SOURCES= cAstXml.cpp \
	cBinaryExpr.cpp \
	cCodeGen.cpp \
	cDecl.cpp \
	cGenAddr.cpp \
	cSemantics.cpp \
	cSizeOffset.cpp \
	cSymbolTable.cpp \
	cTypeDecl.cpp \
	cVisitor.cpp \
	stacklc.cpp \

OBJS= scanner.o \
	 parser.o \
	 cSymbolTable.o \
	 cDecl.o \
	 cBinaryExpr.o \
	 stacklc.o \
	 cVisitor.o \
	 cAstXml.o \
	 cSizeOffset.o \
	 cGenAddr.o \
	 cCodeGen.o \
	 cTypeDecl.o \
	 cSemantics.o \

.PHONY: all clean depend

all: depend stacklc

clean:
	rm -f $(OBJS)
	rm -f scanner.c
	rm -f parser.c
	rm -f parser.h
	rm -f stacklc
	rm -f out
	rm -f depend.mk

depend: depend.mk

include depend.mk

depend.mk: parser.h $(SOURCES)
	gcc -MM $(SOURCES) > depend.mk

parser.o: parser.h
	g++ $(CFLAGS) parser.c -o $@ -DYYDEBUG=1

stacklc.o: stacklc.cpp parser.o scanner.o 
	g++ $(CFLAGS) stacklc.cpp -o $@

scanner.c: stackl.l parser.h
	flex -o scanner.c stackl.l

parser.h: stackl.y
	bison --defines=parser.h stackl.y -o parser.c

scanner.o: scanner.c 
	g++ $(CFLAGS) -Wno-sign-compare scanner.c -o $@

stacklc: $(OBJS)
	g++ $(OBJS) -o stacklc

