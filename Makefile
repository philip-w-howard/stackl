RELEASE  = ~/bin
INCLUDES = startup.h \
	   system.h \
	   string.h \
	   syscodes.h \
	   machine_def.h \
	   opcode_defs.h \
	   pio_term.h \
	   dma_term.h \
	   disk.h \

GIT_VERSION = $(shell git describe --always --tags --dirty="-dev")

.PHONY: compiler interp utils

all: version compiler interp utils execs

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
	rm -f test/*.sl
	rm -f test/*.slb

version: .git/refs/heads
	echo "#pragma once" > version.h
	echo "#define VERSION \"$(GIT_VERSION)\"" >> version.h

compiler: version interp
	$(MAKE) -C compiler

interp:  version
	$(MAKE) -C interp

utils:  version
	$(MAKE) -C utils

execs: compiler interp 
	cp compiler/stacklc .
	cp interp/stackl .
	cp interp/syscodes.h .
	cp interp/opcode_defs.h .
	cp interp/machine_def.h .
	cp interp/pio_term.h .
	cp interp/dma_term.h .
	cp interp/disk.h .
	cp utils/makebin .
	cp utils/copy2disk .
