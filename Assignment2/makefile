CC=gcc
CFLAGS=-I.
DEPS = summerSpectacular.h, fedOops.h
SS_OBJ = summerSpectacular.o
FO_OBJ = fedOops.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

summerSpectacular: $(SS_OBJ)
	$(CC) -pthread -o $@ $^ $(CFLAGS) $(LIBS)

fedOops: $(FO_OBJ)
	$(CC) -pthread -o $@ $^ $(CFLAGS) $(LIBS)

all:
	make summerSpectacular
	make fedOops
	make clean

.PHONY: clean

clean:
	rm -f *.o
	rm summerSpectacular
	rm fedOops
