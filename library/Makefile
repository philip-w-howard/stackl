SLS = string.sl \
      sysio.sl \

all: $(SLS)

clean: 
	rm -f *.sl

%.sl : %.c
	../stacklc -c $? 
