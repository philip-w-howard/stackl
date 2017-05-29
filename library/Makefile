INTERP_DIR = ../interp
INCLUDES = pio_term.h \
	   timer.h \
	   gen_io.h \
	   disk.h \
	   syscodes.h \
	   machine_def.h \

SLS = string.sl \
      sysio.sl \

all: $(INCLUDES) $(SLS)

clean: 
	rm -f *.sl
	rm -f $(INCLUDES)

%.sl : %.c
	../stacklc -c $? 

pio_term.h : $(INTERP_DIR)/pio_term.h
	cp $< $@

timer.h : $(INTERP_DIR)/timer.h
	cp $< $@

gen_io.h : $(INTERP_DIR)/gen_io.h
	cp $< $@

disk.h : $(INTERP_DIR)/disk.h
	cp $< $@

syscodes.h : $(INTERP_DIR)/syscodes.h
	cp $< $@

machine_def.h : $(INTERP_DIR)/machine_def.h
	cp $< $@
