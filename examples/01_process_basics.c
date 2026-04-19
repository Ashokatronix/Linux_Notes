/*
 * 01_process_basics.c - PID, PPID, fork(), process states
 *
 * Compile: gcc 01_process_basics.c -o process_basics
 * Run    : ./process_basics
 * Ref    : man 2 fork, man 2 getpid, man 2 wait
 *
 * Topics:
 *   - What is a Process?  A running instance of a program with its own
 *     memory space, PID, file descriptors, etc.
 *   - PID  : Process ID  – unique integer assigned by kernel (1 to max_pid)
 *   - PPID : Parent PID  – the process that created this one
 *   - fork(): creates an exact copy (child). Returns 0 in child, child-PID
 *            in parent, -1 on error.
 *   - Process States: Running, Sleeping (S), Waiting (D), Stopped (T),
 *                     Zombie (Z)
 */

#include <stdio.h>
#include <unistd.h>   /* fork, getpid, getppid, sleep */
#include <sys/wait.h> /* wait() */
#include <stdlib.h>

int main(void)
{
    printf("=== Process Basics ===\n");
    printf("[Parent] PID=%d  PPID=%d\n", getpid(), getppid());

    pid_t child = fork();   /* <-- kernel creates child here */

    if (child < 0) {
        perror("fork failed");
        return 1;
    }

    if (child == 0) {
        /* ------- CHILD process runs this block ------- */
        printf("[Child ] PID=%d  PPID=%d\n", getpid(), getppid());
        sleep(1);           /* simulate some work */
        printf("[Child ] Done. Exiting.\n");
        exit(0);
    } else {
        /* ------- PARENT process runs this block ------ */
        printf("[Parent] Spawned child PID=%d\n", child);
        wait(NULL);         /* wait for child – avoids zombie state */
        printf("[Parent] Child finished. Parent exiting.\n");
    }

    return 0;
}
