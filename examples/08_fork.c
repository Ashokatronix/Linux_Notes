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
    /* This line runs ONCE — before the process is duplicated */
    printf("[before fork] PID=%d\n", getpid());

    /* fork() duplicates this entire process here.
     * TWO processes continue from the NEXT line.
     * Return value tells them apart:
     *   parent → child's PID (> 0)
     *   child  → 0
     *   error  → -1
     * COW: both share physical memory pages until one writes — makes fork() fast. */
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        /* ── CHILD: fork() returned 0 ───────────────────────────────────── */
        printf("[CHILD ] PID=%d  PPID=%d\n", getpid(), getppid());
        printf("[CHILD ] Doing some work...\n");
        sleep(1);
        printf("[CHILD ] Done. Exiting.\n");
        exit(0);   /* exit code 0 = success; parent collects this via WEXITSTATUS */

    } else {
        /* ── PARENT: fork() returned child's PID ─────────────────────────── */
        printf("[PARENT] PID=%d  spawned child PID=%d\n", getpid(), pid);

        /* wait() blocks parent until child exits.
         * Without this, child becomes a ZOMBIE (Z state) —
         * dead but its process table entry lingers until parent reaps it. */
        int status;
        wait(&status);

        /* WIFEXITED  → true if child exited normally (not killed by signal)
         * WEXITSTATUS → extracts the actual exit code (0 = success) */
        if (WIFEXITED(status))
            printf("[PARENT] Child exited with code %d\n", WEXITSTATUS(status));
    }

    return 0;
}
