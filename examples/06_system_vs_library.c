/*
 * 06_system_vs_library.c - system() call, library vs system calls,
 *                          kernel space vs user space
 *
 * Compile: gcc 06_system_vs_library.c -o syscall_demo
 * Run    : ./syscall_demo
 * Ref    : man 2 write, man 2 read, man 3 printf, man 2 syscall
 *
 * ─────────────────────────────────────────────────────────────────
 * USER SPACE  │  Library Call (e.g. printf)  → libc → system call
 * ────────────┼───────────────────────────────────────────────────
 * KERNEL SPACE│  System Call (e.g. write())  → kernel executes
 * ─────────────────────────────────────────────────────────────────
 *
 * Library calls (printf, fopen, malloc) run in USER SPACE.
 *   They often buffer data and eventually call a system call.
 *
 * System calls (write, open, read, fork) cross into KERNEL SPACE.
 *   They are slower (context switch) but have direct kernel access.
 *
 * system("cmd") :
 *   - Creates a child process via fork()
 *   - Runs /bin/sh -c "cmd" via execve()
 *   - Parent waits with wait()
 *   - Sequential – does NOT return until shell command finishes
 *   - Expensive: fork + exec + wait
 */

#include <stdio.h>
#include <unistd.h>    /* write() system call     */
#include <string.h>
#include <stdlib.h>    /* system()                */

int main(void)
{
    /* -------- Library call: printf (buffered, user-space) -------- */
    printf("Hello from printf() [library call - user space]\n");
    fflush(stdout);

    /* -------- System call: write() (unbuffered, crosses to kernel) */
    const char *msg = "Hello from write() [system call - kernel space]\n";
    write(STDOUT_FILENO, msg, strlen(msg));  /* sysno 1 on x86-64 */

    /* -------- system() – sequential, runs shell command ----------- */
    printf("\n--- system() examples (sequential) ---\n");
    system("echo 'system(): running ls -l /tmp'");
    system("ls -l /tmp | head -5");   /* returns only after ls finishes */

    printf("\n--- Done ---\n");
    return 0;
}
