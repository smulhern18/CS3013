CC=gcc
CFLAGS=-I.
DEPS = helloworld.h
OBJ = helloworld.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

helloworld: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
	make clean

.PHONY: clean

clean:
	rm -f *.o