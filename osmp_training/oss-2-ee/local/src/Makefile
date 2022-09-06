
CC=gcc
CFLAGS=-O3

all: ../lib/ee_tools.o test

test: test.c ../lib/ee_tools.o
	$(CC) $(CFLAGS) $^ -o $@ 

../lib/ee_tools.o: ee_tools.c
	$(CC) -c $(CFLAGS) $^ -o $@ 

clean:
	rm -fr *.o test ../lib/*.o

wipe: clean
	rm -fr *.err *.out
