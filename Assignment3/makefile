CC=gcc
CFLAGS=-I.
DEPS = goatmalloc.h
OBJ = test_goatmalloc.o, goatmalloc.o

test_goatmalloc:
	gcc test_goatmalloc.c goatmalloc.c -o test_goatmalloc

all:
	make test_goatmalloc
	make clean

.PHONY: clean

clean:
	rm -f *.o