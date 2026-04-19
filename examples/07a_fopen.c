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
    /* ── WRITE ─────────────────────────────────────── */
    FILE *f = fopen(FILE_PATH, "w");   /* create / overwrite */
    if (!f) {
        perror("fopen write");
        return 1;
    }

    fprintf(f, "Line 1: Hello from fopen!\n");
    fprintf(f, "Line 2: PID of writer = implicit (this process)\n");
    fprintf(f, "Line 3: fopen uses buffered I/O\n");

    fclose(f);   /* flushes buffer to kernel, then closes fd */
    printf("[fopen] Written to %s\n", FILE_PATH);

    /* ── READ BACK ──────────────────────────────────── */
    f = fopen(FILE_PATH, "r");
    if (!f) {
        perror("fopen read");
        return 1;
    }

    char buf[128];
    printf("[fopen] Reading back:\n");
    while (fgets(buf, sizeof(buf), f))   /* reads one line at a time */
        printf("  %s", buf);

    fclose(f);
    return 0;
}
