# TCP-Chat-Application

## Features
- TCP-based client–server communication
- Handles multiple messages between client and server
- Uses multi-threading and sockets


## How It Works
The server listens on a specified port and waits for client connections. The client connects to the server using an IP address and port. Messages are sent from the client to the server and then the server sends those messages to the rest of the clients. The client.c and server.c files use a multi-threaded design to listen and send messages concurrently.


## How to Compile with Makefile
- make server
- make client


## How to run
1. Run the server with: ./server
2. Run the client with: ./client (run as many instances of the client as you want)
