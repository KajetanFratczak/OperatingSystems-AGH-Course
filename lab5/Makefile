CC = gcc
CFLAGS = -Wall -std=c17

all: zad1 catcher sender

zad1.o: zad1.c
	$(CC) $(CFLAGS) -c zad1.c
zad1: zad1.o
	$(CC) $(CFLAGS) zad1.o -o zad1

catcher.o: catcher.c
	$(CC) $(CFLAGS) -c catcher.c
catcher: catcher.o
	$(CC) $(CFLAGS) catcher.o -o catcher

sender.o: sender.c
	$(CC) $(CFLAGS) -c sender.c
sender: sender.o
	$(CC) $(CFLAGS) sender.o -o sender

clean:
	-rm *.o zad1 catcher sender

.PHONY: all clean
