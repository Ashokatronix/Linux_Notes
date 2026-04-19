/*
 * 07c_socket.c - Simple TCP server that accepts one client connection
 *
 * Compile : gcc 07c_socket.c -o socket_demo
 * Run     : ./socket_demo &          (start server in background)
 *           echo "Hello" | nc localhost 9090   (test with netcat)
 *        OR: telnet localhost 9090
 * Ref     : man 2 socket, man 2 bind, man 2 listen, man 2 accept
 *
 * Socket lifecycle (server side):
 *   socket() → bind() → listen() → accept() → read/write → close()
 *
 * AF_INET      = IPv4 address family
 * SOCK_STREAM  = TCP (reliable, stream-based)
 * htons()      = host-to-network byte order (big-endian) for port
 * INADDR_ANY   = bind to all available network interfaces
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT    9090
#define BACKLOG 5      /* max pending connections in listen queue */

int main(void)
{
    /* Step 1: Create socket file descriptor */
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("socket"); return 1; }

    /* Allow port reuse (avoids "Address already in use" on restart) */
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    /* Step 2: Bind to address + port */
    struct sockaddr_in addr = {
        .sin_family      = AF_INET,
        .sin_port        = htons(PORT),
        .sin_addr.s_addr = INADDR_ANY
    };
    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind"); return 1;
    }

    /* Step 3: Start listening */
    listen(server_fd, BACKLOG);
    printf("[server] Listening on port %d  (try: echo Hi | nc localhost %d)\n",
           PORT, PORT);

    /* Step 4: Accept one client */
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd < 0) { perror("accept"); return 1; }

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
    printf("[server] Client connected from %s:%d\n",
           client_ip, ntohs(client_addr.sin_port));

    /* Step 5: Read data from client */
    char buf[256] = {0};
    int n = read(client_fd, buf, sizeof(buf) - 1);
    printf("[server] Received %d bytes: %s", n, buf);

    /* Step 6: Send reply */
    const char *reply = "Hello from server! Goodbye.\n";
    write(client_fd, reply, strlen(reply));

    /* Step 7: Close connections */
    close(client_fd);
    close(server_fd);
    printf("[server] Done.\n");
    return 0;
}
