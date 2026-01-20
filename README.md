# Concurrent TCP Chat Server (C) — `select()` Multi-Client

A simple **TCP server and client** written in C that demonstrates how to handle **multiple concurrent clients on a single port** using the `select()` system call.

This project focuses on core socket programming concepts and clean shutdown handling.

---

## What This Project Is About

This project is a minimal but complete example of:

- TCP socket programming in C
- Handling multiple clients without threads
- Using `select()` for I/O multiplexing
- Graceful server shutdown with signal handling (`SIGINT`)

It is essentially designed for learning and demonstration purposes during my System Programming class, especially for networking fundamentals.

---

## Features

### Server
- Listens on a single TCP port (`8080`)
- Supports multiple clients concurrently using `select()`
- Prints messages received from any client
- Gracefully shuts down on **Ctrl+C**
  - Sends `"Server shutting down"` to all connected clients
  - Closes all client sockets and the listening socket

### Client
- Connects to `127.0.0.1:8080`
- Allows continuous message sending
- Automatically exits if:
  - The server shuts down
  - The connection is lost
- Manual exit using **Ctrl+C**

---

## Requirements

- GCC or Clang
- Linux, macOS, or WSL (recommended)
- Basic familiarity with terminal usage

---

## Files

- `server-1.c` — Concurrent TCP server using `select()`
- `client-1.c` — Interactive TCP client

---

## Build Instructions

```bash
gcc server-1.c -o server
gcc client-1.c -o client
```

---

## Running the Program

### Start the Server
```bash
./server
```

### Start One or More Clients (in separate terminals)
```bash
./client
```

---

## How To Use

1. Run the server.
2. Launch one or more clients in different terminals.
3. Type messages at the `Send>` prompt.
4. Observe messages printed on the server.
5. Stop the server using **Ctrl+C**.
   - All connected clients will receive a shutdown message and exit cleanly.

---

## Example Output

### Server
```text
Server listening on port 8080 (Press Ctrl+C to stop)
New connection: 127.0.0.1:53021
Received: Hello from client
^C
SIGINT caught — notifying all clients and closing server.
Server closed successfully.
```

### Client
```text
Connected to server 127.0.0.1:8080
Send> Hello from client

[Notice] Server shutting down
Client closed.
```

---

## Configuration

### Server Port (`server-1.c`)
```c
#define LISTEN_PORT 8080
```

### Server Address (`client-1.c`)
```c
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
```

---

## Limitations

- The server does **not broadcast messages** to other clients.
- Designed for learning purposes, not production use.
- `select()` is suitable for small-scale concurrency.

---

## Author

Edward Moon
