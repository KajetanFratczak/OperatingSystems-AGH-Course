CC = gcc
CFLAGS = -Wall -std=c17 -D_POSIX_C_SOURCE=200809L

all: user printer init

user.o: user.c
	$(CC) $(CFLAGS) -c user.c
user: user.o
	$(CC) $(CFLAGS) user.o -o user

printer.o: printer.c
	$(CC) $(CFLAGS) -c printer.c
printer: printer.o
	$(CC) $(CFLAGS) printer.o -o printer

init.o: init.c
	$(CC) $(CFLAGS) -c init.c
init: init.o
	$(CC) $(CFLAGS) init.o -o init

clean:
	-rm *.o user printer init

.PHONY: all clean
