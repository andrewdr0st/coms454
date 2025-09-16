CC = gcc
CFLAGS = -Wall
LIB = -lssl -lcrypto

all: server client

server: server.c
	$(CC) $(CFLAGS) server.c -o server $(LIB)

client: client.c
	$(CC) $(CFLAGS) client.c -o client $(LIB)

clean:
	rm -f server client

