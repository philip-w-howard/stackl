INTERP_DIR = ../interp
INCLUDES = pio_term.h \
	   timer.h \
	   gen_io.h \
	   disk.h \
	   inp_def.h \
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

inp_def.h : $(INTERP_DIR)/inp_def.h
	cp $< $@

machine_def.h : $(INTERP_DIR)/machine_def.h
	cp $< $@
