CC=gcc
CFLAGS=-I.
DEPS = scheduler.h
OBJ = scheduler.o

scheduler:
	gcc scheduler.c -o scheduler

all:
	make scheduler
	make clean

.PHONY: clean

clean:
	rm -f *.o
