OBJS = mymalloc.o
CC = gcc
CFLAGS = -g -Wall

#list of optional tests
TESTS = simpletest errorchecking 
#make tests executable
all: $(TESTS)

simpletest: simpletest.o mymalloc.o 
	$(CC) $(CFLAGS) -o $@ $^

errorchecking: errorchecking.o mymalloc.o 
	$(CC) $(CFLAGS) -o $@ $^

#compile tests
%.o: tests/%.c mymalloc.o
	$(CC) $(CFLAGS) -c $<

#objects
mymalloc.o: mymalloc.c mymalloc.h
	$(CC) $(CFLAGS) -c $<


.PHONY: clean
clean:
	rm -f $(TESTS) *.o
