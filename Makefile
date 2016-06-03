#RELEASE  = ~/stackl_rel
RELEASE  = ~/bin
# RELEASE  = release
EXTRA_INCLUDES = \
	   system.h \
	   opcode_defs.h \
	   dma_term.h \
	   disk.h \

INCLUDES = string.h \
	   machine_def.h \
	   syscodes.h \
	   pio_term.h \
	   timer.h \

GIT_VERSION = $(shell git describe --always --tags --dirty="-dev")

.PHONY: compiler interp utils

all: version compiler interp utils includes execs

release: all
	cp copy2disk $(RELEASE)
	cp makedisk $(RELEASE)
	cp slasm $(RELEASE)
	cp stackl $(RELEASE)
	cp stacklc $(RELEASE)
	cp $(INCLUDES) $(RELEASE)
	chmod 755 $(RELEASE)/copy2disk
	chmod 755 $(RELEASE)/makedisk
	chmod 755 $(RELEASE)/slasm
	chmod 755 $(RELEASE)/stackl
	chmod 755 $(RELEASE)/stacklc
	chmod 644 $(RELEASE)/*.h
clean:
	$(MAKE) -C compiler clean
	$(MAKE) -C interp clean
	$(MAKE) -C utils clean
	rm -f slasm
	rm -f stackl
	rm -f stacklc
	rm -f copy2disk
	rm -f makedisk
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
	cp interp/slasm .
	cp interp/stackl .
	cp utils/copy2disk .
	cp utils/makedisk .

includes:
	cp interp/syscodes.h .
	cp interp/machine_def.h .
	cp interp/pio_term.h .
	cp interp/timer.h .

extra_includes: compiler interp 
	cp interp/syscodes.h .
	cp interp/opcode_defs.h .
	cp interp/pio_term.h .
	cp interp/dma_term.h .
	cp interp/disk.h .

