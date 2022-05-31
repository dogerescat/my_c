CFLAG=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

my_c: $(OBJS)
	$(CC) -o my_c $(OBJS) $(LDFLAGS)

$(OBJS): my_c.h

test: my_c
	./test.sh

clean: 
	rm -f my_c *.o *~ tmp* *.s

.PHONY: test clean
