/*
 * getpid.c - Demonstrates getpid() and getppid() system calls
 *
 * Concepts: Process ID (PID), Parent Process ID (PPID)
 * Compile : gcc getpid.c -o getpid
 * Run     : ./getpid
 * Ref     : man 2 getpid
 */

#include <unistd.h>  /* getpid(), getppid() */
#include <stdio.h>   /* printf()            */

int main()
{
    /* getpid() - returns PID of the calling process */
    pid_t pid  = getpid();

    /* getppid() - returns PID of the parent process (e.g. bash shell) */
    pid_t ppid = getppid();

    printf("My  Process ID (PID)  : %d\n", pid);
    printf("My Parent's PID (PPID): %d\n", ppid);

    return 0;
}
