/*
 * 07b_fork.c - Creating a child process with fork()
 *
 * Compile : gcc 07b_fork.c -o fork_demo
 * Run     : ./fork_demo
 * Ref     : man 2 fork, man 2 wait
 *
 * fork() duplicates the calling process:
 *   - Returns 0       in the CHILD
 *   - Returns child's PID in the PARENT
 *   - Returns -1 on error
 *
 * Both parent and child continue executing from the line AFTER fork().
 * Child inherits: open file descriptors, env vars, memory (Copy-on-Write).
 *
 * Copy-on-Write (COW):
 *   After fork(), parent and child share the same physical memory pages.
 *   A private copy is made only when either side actually WRITES to a page.
 *   This makes fork() very fast.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>    /* fork(), getpid(), getppid() */
#include <sys/wait.h>  /* wait() */

int main(void)
{
    printf("[before fork] PID=%d\n", getpid());

    pid_t pid = fork();   /* <── kernel duplicates process here */

    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        /* ── CHILD ─────────────────────────────────── */
        printf("[CHILD ] PID=%d  PPID=%d\n", getpid(), getppid());
        printf("[CHILD ] Doing some work...\n");
        sleep(1);
        printf("[CHILD ] Done. Exiting.\n");
        exit(0);

    } else {
        /* ── PARENT ────────────────────────────────── */
        printf("[PARENT] PID=%d  spawned child PID=%d\n", getpid(), pid);

        /* wait() blocks until child exits – prevents zombie state */
        int status;
        wait(&status);

        if (WIFEXITED(status))
            printf("[PARENT] Child exited with code %d\n", WEXITSTATUS(status));
    }

    return 0;
}
