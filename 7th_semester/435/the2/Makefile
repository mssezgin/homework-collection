CC=g++
CFLAGS=-c -pedantic-errors -O0 -std=c++11

all: server client

server: server.o
	$(CC) -o server server.o

client: client.o
	$(CC) -o client client.o

server.o: server.cpp
	$(CC) $(CFLAGS) -o server.o server.cpp

client.o: client.cpp
	$(CC) $(CFLAGS) -o client.o client.cpp

clean:
	rm server.o
	rm client.o
	rm server
	rm client

tcp: tcp_server tcp_client

tcp_server: tcp_server.o
	$(CC) -o tcp_server tcp_server.o

tcp_client: tcp_client.o
	$(CC) -o tcp_client tcp_client.o

tcp_server.o: tcp_server.cpp
	$(CC) $(CFLAGS) -o tcp_server.o tcp_server.cpp

tcp_client.o: tcp_client.cpp
	$(CC) $(CFLAGS) -o tcp_client.o tcp_client.cpp

tcp_clean:
	rm tcp_server.o
	rm tcp_client.o
	rm tcp_server
	rm tcp_client
