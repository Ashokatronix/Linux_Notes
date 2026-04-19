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

    /* getpid()  → PID of this process (assigned by kernel, system-wide unique)
     * getppid() → PID of whoever launched this (e.g. bash) */
    printf("[Parent] PID=%d  PPID=%d\n", getpid(), getppid());

    /* fork() duplicates the entire process here.
     * After this line TWO processes run the same code.
     * Return value tells them apart:
     *   parent → child's PID (> 0)
     *   child  → 0
     *   error  → -1 */
    pid_t child = fork();

    if (child < 0) {
        perror("fork failed");
        return 1;
    }

    if (child == 0) {
        /* ── CHILD process runs this block ──────────────────────────
         * child == 0 because fork() returns 0 in the child.
         * PPID here = parent's PID, proving the relationship. */
        printf("[Child ] PID=%d  PPID=%d\n", getpid(), getppid());
        sleep(1);           /* simulate work (child enters 'S' sleep state) */
        printf("[Child ] Done. Exiting.\n");
        exit(0);            /* child exits cleanly */

    } else {
        /* ── PARENT process runs this block ─────────────────────────
         * child > 0 here — it's the child's PID returned by fork(). */
        printf("[Parent] Spawned child PID=%d\n", child);

        /* wait(NULL) blocks parent until child exits.
         * Without this, the child becomes a ZOMBIE (Z state) —
         * dead but its process table entry stays until parent reaps it. */
        wait(NULL);
        printf("[Parent] Child finished. Parent exiting.\n");
    }

    return 0;
}
