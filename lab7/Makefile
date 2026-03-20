CC = gcc
CFLAGS = -Wall -std=c17

all: client server

client.o: client.c
	$(CC) $(CFLAGS) -c client.c
client: client.o
	$(CC) $(CFLAGS) client.o -o client

server.o: server.c
	$(CC) $(CFLAGS) -c server.c
server: server.o
	$(CC) $(CFLAGS) server.o -o server

clean:
	-rm *.o client server

.PHONY: all clean
