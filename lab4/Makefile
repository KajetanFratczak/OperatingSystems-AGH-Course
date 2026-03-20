CC = gcc
CFLAGS = -Wall -std=c17

all: zad1 zad2

zad1.o: zad1.c
	$(CC) $(CFLAGS) -c zad1.c

zad1: zad1.o
	$(CC) $(CFLAGS) zad1.o -o zad1

zad2.o: zad2.c
	$(CC) $(CFLAGS) -c zad2.c

zad2: zad2.o
	$(CC) $(CFLAGS) zad2.o -o zad2

clean:
	rm *.o zad1 zad2

.PHONY: all clean