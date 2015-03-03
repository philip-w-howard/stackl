.PHONY compiler interp

all: compiler interp execs

clean:
	$(MAKE) -C compiler clean
	$(MAKE) -C interp clean
	rm -f stackl
	rm -f stacklc

compiler:
	$(MAKE) -C compiler

interp: 
	$(MAKE) -C interp

execs: compiler interp
	cp compiler/stacklc .
	cp interp/stackl .
