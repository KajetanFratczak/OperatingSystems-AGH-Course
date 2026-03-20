CC = gcc
CFLAGS = -Wall -std=c17

all: zad1 zad2_1 zad2_2

zad1.o: zad1.c
	$(CC) $(CFLAGS) -c zad1.c
zad1: zad1.o
	$(CC) $(CFLAGS) zad1.o -o zad1

zad2_1.o: zad2_1.c
	$(CC) $(CFLAGS) -c zad2_1.c
zad2_1: zad2_1.o
	$(CC) $(CFLAGS) zad2_1.o -o zad2_1

zad2_2.o: zad2_2.c
	$(CC) $(CFLAGS) -c zad2_2.c
zad2_2: zad2_2.o
	$(CC) $(CFLAGS) zad2_2.o -o zad2_2

clean:
	-rm *.o zad1 zad2_1 zad2_2

.PHONY: all clean
