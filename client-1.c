/*
 ============================================================================
 Name        : client.c
 Author      : Edward Moon
 Assignment  : Module 7 Programming Assignment
 Description : TCP Client for Concurrent Server
 ============================================================================
PURPOSE:
   Connects to the concurrent TCP server and allows the user to send
   messages continuously. The client exits automatically when the server
   shuts down or becomes unreachable.

 INSTRUCTIONS:
   1. Compile: gcc client.c -o client
   2. Run:     ./client
   3. Type messages and press Enter to send.
   4. If server shuts down, the client exits automatically.
 ============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>

#define BUF_SIZE 1024
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080

/*
 * handle_sigint:
 * Gracefully exits when Ctrl+C is pressed.
 */
void handle_sigint(int sig)
{
    (void)sig;
    printf("\nClient terminated manually.\n");
    exit(0);
}

/*
 * main:
 * Connects to the server and lets the user send messages repeatedly.
 * Exits if the server disconnects or sends a shutdown message.
 */
int main(void)
{
    int sock, bytes_sent, bytes_recv;
    char text[BUF_SIZE], buf[BUF_SIZE];
    struct sockaddr_in addr;

    signal(SIGINT, handle_sigint);

    // We create socket for the client
    sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0)
    {
        perror("socket() failed");
        exit(1);
    }

    // We define server address to make the connection
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    addr.sin_port = htons(SERVER_PORT);

    // We finally connect to the server
    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("connect() failed");
        exit(1);
    }

    printf("Connected to server %s:%d\n", SERVER_IP, SERVER_PORT);
    printf("Type messages and press Enter. Ctrl+C to quit.\n");

    // Continuous send loop so that we don't just send 1 msg and stop
    while (1)
    {
        printf("Send> ");
        fflush(stdout);

        // Read user input
        if (fgets(text, BUF_SIZE, stdin) == NULL)
            break;

        text[strcspn(text, "\n")] = '\0'; // Remove newline
        if (strlen(text) == 0)
            continue;

        // Send message to server
        bytes_sent = send(sock, text, strlen(text), 0);
        if (bytes_sent <= 0)
        {
            printf("\nServer unreachable.\n");
            break;
        }

        // Non-blocking check if server is shutting down
        bytes_recv = recv(sock, buf, BUF_SIZE - 1, MSG_DONTWAIT);
        if (bytes_recv > 0)
        {
            buf[bytes_recv] = '\0';
            if (strstr(buf, "Server shutting down"))
            {
                printf("\n[Notice] %s\n", buf);
                break;
            }
        }
    }

    close(sock);
    printf("Client closed.\n");
    return 0;
}
