RELEASE  = ~/bin
INCLUDES = startup.h \
	   system.h \
	   string.h \
	   syscodes.h \
	   machine_def.h \
	   dma_term.h \
	   disk.h \

.PHONY: compiler interp utils

all: compiler interp utils execs

release: all
	cp copy2disk $(RELEASE)
	cp makebin $(RELEASE)
	cp stackl $(RELEASE)
	cp stacklc $(RELEASE)
	cp $(INCLUDES) $(RELEASE)
clean:
	$(MAKE) -C compiler clean
	$(MAKE) -C interp clean
	$(MAKE) -C utils clean
	rm -f stackl
	rm -f stacklc
	rm -f makebin
	rm -f copy2disk
	rm -f out

compiler:
	$(MAKE) -C compiler

interp: 
	$(MAKE) -C interp

utils: 
	$(MAKE) -C utils

execs: compiler interp 
	cp compiler/stacklc .
	cp interp/stackl .
	cp interp/syscodes.h .
	cp interp/machine_def.h .
	cp interp/dma_term.h .
	cp interp/disk.h .
	cp utils/makebin .
	cp utils/copy2disk .
