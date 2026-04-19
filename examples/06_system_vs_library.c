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
    /* ── Library call: printf() ─────────────────────────────────────────
     * Runs entirely in USER SPACE inside libc.
     * Data goes into a buffer first — does NOT immediately cross to kernel.
     * Actual write() syscall happens on: fflush(), buffer full, or fclose(). */
    printf("Hello from printf() [library call - user space]\n");
    fflush(stdout);  /* force buffer flush → triggers write() syscall now */

    /* ── System call: write() ───────────────────────────────────────────
     * Crosses directly into KERNEL SPACE — no buffering.
     * STDOUT_FILENO = fd 1 (the terminal).
     * On x86-64 this maps to syscall number 1.
     * Slower per-call than printf if called in a tight loop,
     * but data goes to kernel immediately (no buffering delay). */
    const char *msg = "Hello from write() [system call - kernel space]\n";
    write(STDOUT_FILENO, msg, strlen(msg));

    /* ── system() – the most expensive: fork + exec + wait ─────────────
     * Each call to system() internally does:
     *   fork()   → create child process
     *   execve() → child becomes /bin/sh running the command
     *   wait()   → parent BLOCKS until shell finishes
     * Because of wait(), execution is purely SEQUENTIAL —
     * the next line won't run until ls fully completes. */
    printf("\n--- system() examples (sequential) ---\n");
    system("echo 'system(): running ls -l /tmp'");
    system("ls -l /tmp | head -5");   /* blocks here until ls exits */

    printf("\n--- Done ---\n");  /* only prints after both system() calls return */
    return 0;
}
