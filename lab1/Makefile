CC = gcc
CFLAGS = -Wall -std=c17 -g

all: countdown

countdown: countdown.o
	$(CC) countdown.o -o countdown

countdown.o: countdown.c
	$(CC) $(CFLAGS) -c countdown.c

clean:
	rm *.o countdown

.PHONY: clean