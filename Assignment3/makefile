CC=gcc
CFLAGS=-I.
DEPS = goatmalloc.h
OBJ = test_goatmalloc.o, goatmalloc.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

test_goatmalloc: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

all:
	make test_goatmalloc
	make clean

.PHONY: clean

clean:
	rm -f *.o