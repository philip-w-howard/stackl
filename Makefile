#RELEASE  = ~/stackl_rel
RELEASE  = ~/bin
# RELEASE  = release
EXTRA_INCLUDES = \
	   system.h \
	   opcode_defs.h \
	   dma_term.h \
	   disk.h \

INCLUDES = machine_def.h \
	   syscodes.h \
	   pio_term.h \
	   timer.h \

GIT_VERSION = $(shell git describe --always --tags --dirty="-dev")

.PHONY: compiler interp utils libraries

all: version compiler interp libraries utils includes execs 

release: all
	( [ ! -e $(RELEASE)/library ] && mkdir $(RELEASE)/library ) || true 
	cp copy2disk $(RELEASE)
	cp makedisk $(RELEASE)
	cp slasm $(RELEASE)
	cp stackl $(RELEASE)
	cp stacklc $(RELEASE)
	cp library/*.h $(RELEASE)/library
	cp library/*.sl $(RELEASE)/library
	chmod 755 $(RELEASE)/copy2disk
	chmod 755 $(RELEASE)/makedisk
	chmod 755 $(RELEASE)/slasm
	chmod 755 $(RELEASE)/stackl
	chmod 755 $(RELEASE)/stacklc
	chmod 644 $(RELEASE)/library/*
clean:
	$(MAKE) -C compiler clean
	$(MAKE) -C interp clean
	$(MAKE) -C utils clean
	$(MAKE) -C library clean
	rm -f slasm
	rm -f stackl
	rm -f stacklc
	rm -f copy2disk
	rm -f makedisk
	rm -f out
	rm -f *.sl
	rm -f test/*.sl
	rm -f test/*.slb
	rm -f test/*.lst
	rm -f test/*.ast
	rm -f test/*.xml
	rm -f test/*.dbg
	rm -f test/*.lst

version: .git/refs/heads
	echo "#pragma once" > version.h
	echo "#define VERSION \"$(GIT_VERSION)\"" >> version.h

compiler: version interp
	$(MAKE) -C compiler

interp:  version
	$(MAKE) -C interp

libraries: compiler execs
	$(MAKE) -C library

utils:  version
	$(MAKE) -C utils

execs: compiler interp utils
	cp compiler/stacklc .
	cp interp/slasm .
	cp interp/stackl .
	cp utils/copy2disk .
	cp utils/makedisk .

includes: libraries
	cp interp/syscodes.h library
	cp interp/machine_def.h library
	cp interp/pio_term.h library
	cp interp/timer.h library

extra_includes: compiler interp 
	cp interp/syscodes.h .
	cp interp/opcode_defs.h .
	cp interp/pio_term.h .
	cp interp/dma_term.h .
	cp interp/disk.h .

