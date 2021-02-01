CC=gcc
CFLAGS=-I.
DEPS = prolific.h generation.h
PROLIFIC_OBJ = prolific.o
GENERATION_OBJ = generation.o
EXPLORER_OBJ = explorer.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

prolific: $(PROLIFIC_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

generation: $(GENERATION_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

explorer: $(EXPLORER_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)


.PHONY: clean

clean:
	rm -f *.o