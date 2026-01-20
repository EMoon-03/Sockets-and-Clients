/*
 ============================================================================
 Name        : server.c
 Author      : Edward Moon
 Assignment  : Module 7 Programming Assignment
 ============================================================================
 PURPOSE:
   This program creates a TCP server that can manage multiple concurrent
   clients on the same port. It uses the select() system call to monitor
   all client sockets simultaneously. Each client can send messages freely.
   When Ctrl+C is pressed, the server sends a "Server shutting down" message
   to every connected client, closes all sockets, and exits gracefully.

 INSTRUCTIONS:
   1. Compile: gcc server.c -o server
   2. Run:     ./server
   3. In other terminals, run multiple clients with ./client
   4. Press Ctrl+C to stop the server (clients will exit automatically)
 ============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <signal.h>
#include <errno.h>

#define BUF_SIZE 1024
#define LISTEN_PORT 8080

int sock_listen;     // Listening socket descriptor
fd_set master_set;   // Master set of all sockets (monitored by select())

/*
 * handle_sigint:
 * Handles Ctrl+C. Sends a "Server shutting down" message to all
 * connected clients, closes their sockets, and terminates cleanly.
 */
void handle_sigint(int sig)
{
    (void)sig;
    char msg[] = "Server shutting down\n";

    printf("\nSIGINT caught — notifying all clients and closing server.\n");

    // Loop through all connected sockets
    for (int sd = 0; sd < FD_SETSIZE; sd++)
    {
        if (FD_ISSET(sd, &master_set) && sd != sock_listen)
        {
            send(sd, msg, strlen(msg), 0);
            close(sd);
        }
    }

    close(sock_listen);
    printf("Server closed successfully.\n");
    exit(0);
}

/*
 * main:
 * Initializes the TCP server, binds to a port, and uses select()
 * to monitor multiple clients at once. Each connected client can
 * send multiple messages continuously until it disconnects or
 * the server is stopped.
 */
int main(void)
{
    int new_sock, sd, max_sd, bytes_received;
    struct sockaddr_in my_addr, recv_addr;
    socklen_t addr_size;
    fd_set readfds;
    char buf[BUF_SIZE];

    // Register Ctrl+C handler for graceful shutdown
    signal(SIGINT, handle_sigint);

    // We create a listening socket
    sock_listen = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock_listen < 0)
    {
        perror("socket() failed");
        exit(1);
    }

    // Here we set up the server address
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    my_addr.sin_port = htons(LISTEN_PORT);

    // Then we bind socket to address/port
    if (bind(sock_listen, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0)
    {
        perror("bind() failed");
        exit(1);
    }

    // Server starts listening for incoming connections
    if (listen(sock_listen, 10) < 0)
    {
        perror("listen() failed");
        exit(1);
    }

    printf("Server listening on port %d (Press Ctrl+C to stop)\n", LISTEN_PORT);

    // We then initialize master set and add the listening socket
    FD_ZERO(&master_set);
    FD_SET(sock_listen, &master_set);
    max_sd = sock_listen;

    // We just loop so that we can infinitely receive messages
    while (1)
    {
        readfds = master_set; // Copy master set for select()

        // Wait for socket activity
        int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if ((activity < 0) && (errno != EINTR))
        {
            perror("select() failed");
            continue;
        }

        // Here is where we watch for new clients trying to connect
        if (FD_ISSET(sock_listen, &readfds))
        {
            addr_size = sizeof(recv_addr);
            new_sock = accept(sock_listen, (struct sockaddr *)&recv_addr, &addr_size);
            if (new_sock < 0)
            {
                perror("accept() failed");
                continue;
            }

            printf("New connection: %s:%d\n",
                   inet_ntoa(recv_addr.sin_addr),
                   ntohs(recv_addr.sin_port));

            // Add client to master set
            FD_SET(new_sock, &master_set);
            if (new_sock > max_sd)
                max_sd = new_sock;
        }

        // Check each connected client for incoming messages
        for (sd = 0; sd <= max_sd; sd++)
        {
            if (sd != sock_listen && FD_ISSET(sd, &readfds))
            {
                bytes_received = recv(sd, buf, BUF_SIZE - 1, 0);

                // Handle disconnection or message
                if (bytes_received <= 0)
                {
                    printf("Client disconnected.\n");
                    close(sd);
                    FD_CLR(sd, &master_set);
                }
                else
                {
                    buf[bytes_received] = '\0';
                    printf("Received: %s\n", buf);
                }
            }
        }
    }
}
