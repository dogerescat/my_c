CFLAG=-std=c11 -g -static

my_c: my_c.c

test: my_c
	./test.sh

clean: 
	rm -f my_c *.o *~ tmp*

.PHONY: test clean
