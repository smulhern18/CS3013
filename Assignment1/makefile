CC=gcc
CFLAGS=-I.
DEPS = prolific.h generation.h explorer.h
PROLIFIC_OBJ = prolific.o
GENERATION_OBJ = generation.o
EXPLORER_OBJ = explorer.o
SLUG_OBJ = slug.o
SLUG_RACE_OBJ = slugRace.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

prolific: $(PROLIFIC_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

generation: $(GENERATION_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

explorer: $(EXPLORER_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

slug: $(SLUG_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

slugrace: $(SLUG_RACE_OBJ)
	make slug
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

all:
	make prolific
	make generation
	make explorer
	make slug
	make slugrace
	make clean

.PHONY: clean

clean:
	rm -f *.o