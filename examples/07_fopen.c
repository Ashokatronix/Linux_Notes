/*
 * 07a_fopen.c - Buffered File I/O with fopen / fclose / fprintf / fgets
 *
 * Compile : gcc 07a_fopen.c -o fopen_demo
 * Run     : ./fopen_demo
 * Ref     : man 3 fopen
 *
 * fopen() is a LIBRARY call (runs in user space).
 * It wraps the open() system call and adds a user-space buffer (FILE*).
 * Writes go into the buffer first; actual kernel write happens on:
 *   - fclose()  → flush + close
 *   - fflush()  → flush only
 *   - buffer full
 *
 * Modes:
 *   "r"  read only (file must exist)
 *   "w"  write only (create / truncate)
 *   "a"  append (create if not exists)
 *   "r+" read + write
 */

#include <stdio.h>
#include <stdlib.h>

#define FILE_PATH "/tmp/fopen_demo.txt"

int main(void)
{
    /* ── WRITE ──────────────────────────────────────────────────────────
     * "w" mode: create the file if it doesn't exist, TRUNCATE if it does.
     * fopen() is a library call — wraps the open() syscall + adds FILE* buffer. */
    FILE *f = fopen(FILE_PATH, "w");
    if (!f) {
        perror("fopen write");  /* prints: fopen write: <errno reason> */
        return 1;
    }

    /* fprintf() writes into the FILE* buffer in USER SPACE — NOT to disk yet.
     * Data is batched here; actual kernel write() happens on fclose()/fflush(). */
    fprintf(f, "Line 1: Hello from fopen!\n");
    fprintf(f, "Line 2: PID of writer = implicit (this process)\n");
    fprintf(f, "Line 3: fopen uses buffered I/O\n");

    /* fclose() does TWO things:
     *   1. fflush() — sends buffered data from user space → kernel → disk
     *   2. close()  — releases the file descriptor
     * If we crashed before fclose(), data in buffer would be LOST. */
    fclose(f);
    printf("[fopen] Written to %s\n", FILE_PATH);

    /* ── READ BACK ──────────────────────────────────────────────────────
     * "r" mode: read-only, file MUST exist (returns NULL otherwise). */
    f = fopen(FILE_PATH, "r");
    if (!f) {
        perror("fopen read");
        return 1;
    }

    char buf[128];
    printf("[fopen] Reading back:\n");

    /* fgets() reads up to sizeof(buf)-1 chars OR until '\n' (whichever first).
     * The '\n' IS included in buf, so printf("  %s", buf) needs no extra \n.
     * Returns NULL at EOF → loop exits. */
    while (fgets(buf, sizeof(buf), f))
        printf("  %s", buf);

    fclose(f);
    return 0;
}
