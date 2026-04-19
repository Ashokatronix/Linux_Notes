/*
 * 07_fopen_fork_socket.c - fopen, fork(), and TCP socket demo
 *
 * Compile: gcc 07_fopen_fork_socket.c -o file_fork_socket
 * Run    : ./file_fork_socket
 * Ref    : man 3 fopen, man 2 fork, man 2 socket, man 2 connect
 *
 * fopen()  – library call, buffered I/O, returns FILE* (user space)
 * open()   – system call, unbuffered, returns file descriptor (int)
 * fork()   – creates child; child inherits open file descriptors
 * socket() – creates a communication endpoint (network/IPC)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <errno.h>

/* ─── Part 1: fopen ──────────────────────────────────────────── */
void demo_fopen(void)
{
    const char *path = "/tmp/linux_notes_test.txt";
    FILE *f = fopen(path, "w");   /* open for writing (creates file) */
    if (!f) { perror("fopen"); return; }

    fprintf(f, "Line 1: written by PID %d\n", getpid());
    fprintf(f, "Line 2: fopen uses buffered I/O\n");
    fclose(f);                     /* flushes buffer + closes */

    printf("[fopen] Wrote to %s\n", path);

    /* Read it back */
    f = fopen(path, "r");
    char buf[128];
    while (fgets(buf, sizeof(buf), f))
        printf("[fopen] Read: %s", buf);
    fclose(f);
}

/* ─── Part 2: fork inheriting file descriptor ────────────────── */
void demo_fork_file(void)
{
    FILE *f = fopen("/tmp/fork_test.txt", "w");
    pid_t child = fork();

    if (child == 0) {
        /* Child: writes to same file (inherited fd) */
        fprintf(f, "Child  PID=%d wrote this\n", getpid());
        fclose(f);
        exit(0);
    } else {
        /* Parent */
        fprintf(f, "Parent PID=%d wrote this\n", getpid());
        fclose(f);
        wait(NULL);
    }
    printf("[fork] Both parent & child wrote to /tmp/fork_test.txt\n");
}

/* ─── Part 3: TCP socket client (connect to httpbin.org) ─────── */
void demo_socket(void)
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) { perror("socket"); return; }

    struct sockaddr_in server = {
        .sin_family = AF_INET,
        .sin_port   = htons(80),
    };
    inet_pton(AF_INET, "93.184.216.34", &server.sin_addr); /* example.com */

    printf("[socket] Attempting TCP connect to 93.184.216.34:80 ...\n");

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == 0) {
        printf("[socket] Connected! Sending HTTP GET...\n");
        const char *req = "GET / HTTP/1.0\r\nHost: example.com\r\n\r\n";
        send(sock, req, strlen(req), 0);

        char resp[256];
        int n = recv(sock, resp, sizeof(resp)-1, 0);
        if (n > 0) {
            resp[n] = '\0';
            printf("[socket] Response (first %d bytes):\n%.80s...\n", n, resp);
        }
    } else {
        printf("[socket] Connect failed (network may be unavailable): %s\n",
               strerror(errno));
    }
    close(sock);
}

int main(void)
{
    printf("=== fopen / fork / socket demo ===\n\n");
    demo_fopen();
    printf("\n");
    demo_fork_file();
    printf("\n");
    demo_socket();
    return 0;
}
