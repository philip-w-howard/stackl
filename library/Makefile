SLS = string.sl \
      sysio.sl \

all: $(SLS)

clean: 
	rm -f $(SLS)

%.sl : %.c
	../stacklc -c $? 
