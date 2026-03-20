CC = gcc
CFLAGS = -Wall -std=c17

all: flipper

flipper.o: flipper.c
	$(CC) $(CFLAGS) -c flipper.c

flipper: flipper.o
	$(CC) $(CFLAGS) flipper.o -o flipper

clean:
	rm *.o flipper

.PHONY: all clean


