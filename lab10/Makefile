CC = gcc
CFLAGS = -Wall -std=c17

all: zad1

zad1.o: zad1.c
	$(CC) $(CFLAGS) -c zad1.c
zad1: zad1.o
	$(CC) $(CFLAGS) zad1.o -o zad1

clean:
	-rm *.o zad1

.PHONY: all clean