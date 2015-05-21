RELEASE  = ~/bin
INCLUDES = startup.h \
	   system.h \
	   string.h \
	   syscodes.h \
	   machine_def.h \
	   dma_term.h \
	   disk.h \

.PHONY: compiler interp

all: compiler interp execs

release: all
	cp makebin $(RELEASE)
	cp stackl $(RELEASE)
	cp stacklc $(RELEASE)
	cp $(INCLUDES) $(RELEASE)
clean:
	$(MAKE) -C compiler clean
	$(MAKE) -C interp clean
	rm -f stackl
	rm -f stacklc
	rm -f out

compiler:
	$(MAKE) -C compiler

interp: 
	$(MAKE) -C interp

execs: compiler interp 
	cp compiler/stacklc .
	cp interp/stackl .
	cp interp/makebin .
	cp interp/syscodes.h .
	cp interp/machine_def.h .
	cp interp/dma_term.h .
	cp interp/disk.h .
