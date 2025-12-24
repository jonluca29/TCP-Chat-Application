CC = gcc
TARGETS = server client

all: $(TARGETS)

server: server.c
	$(CC)  server.c -o server -lpthread

client: client.c
	$(CC)  client.c -o client -lpthread

clean:
	rm -f $(TARGETS)

