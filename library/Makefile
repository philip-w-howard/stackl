SLS = string.sl \
      sysio.sl \

all: $(SLS)

clean: 
	rm -f *.sl
	rm machine_def.h
	rm pio_term.h
	rm syscodes.h
	rm timer.h


%.sl : %.c
	../stacklc -c $? 
