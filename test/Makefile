COPTS=-c
SLS=io.sl \
    string.sl

TESTS=test1.slb test2.slb test3.slb test4.slb test5.slb \
	test6.slb test7.slb test8.slb test9.slb test10.slb \
	test11.slb test12.slb test13.slb test14.slb test15.slb \
	test16.slb test17.slb test18.slb test19.slb test20.slb \
	test21.slb test22.slb test23.slb test24.slb test25.slb \
	test26.slb test27.slb test28.slb test29.slb test30.slb \
	test31.slb test32.slb test33.slb test34.slb test35.slb \
	test36.slb test37.slb test38.slb test39.slb test40.slb \

all: $(TESTS)

clean:
	rm -f *.sl
	rm -f *.slb

%.sl : %.c
	stacklc $(COPTS) $<

%.slb : %.sl io.sl
	slasm $< io.sl

