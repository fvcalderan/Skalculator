CC=cc
CFLAGS=-O3
LDFLAGS=-lm -lX11

all: skalculator.o
	$(CC) $(CFLAGS) -o skalculator skalculator.o $(LDFLAGS)
	rm skalculator.o

.PHONY: clean

clean:
	rm skalculator
