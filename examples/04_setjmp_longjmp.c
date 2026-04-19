/*
 * 04_setjmp_longjmp.c - Non-local jumps using setjmp / longjmp
 *
 * Compile: gcc 04_setjmp_longjmp.c -o setjmp_demo
 * Run    : ./setjmp_demo
 * Ref    : man 3 setjmp, man 3 longjmp
 *
 * setjmp(env)   – saves processor state (registers, stack pointer, PC)
 *                 into 'env'. Returns 0 on first call.
 * longjmp(env, val) – restores the saved state; causes setjmp() to
 *                 return 'val' (never returns 0).
 *
 * Use-cases:
 *   - Error recovery / exception simulation in C
 *   - Escaping deeply nested call stacks
 *
 * WARNING:
 *   - Local variables in the function that called setjmp() may be
 *     corrupted (clobbered) after longjmp() unless declared 'volatile'.
 *   - Avoid longjmp across signal handlers (use siglongjmp instead).
 */

#include <stdio.h>
#include <setjmp.h>   /* setjmp, longjmp, jmp_buf */

static jmp_buf jump_buffer;  /* stores CPU state: registers, stack pointer, program counter */

void risky_function(int depth)
{
    if (depth == 0) {
        /* Base case: error condition reached deep in the call stack.
         * Instead of returning error codes up through every caller,
         * longjmp() teleports execution directly back to setjmp() in main.
         * The entire call stack (depth 3→2→1→0) is unwound in one shot.
         * This is C's version of a 'throw' in try/catch. */
        printf("[risky_function] Encountered an error! Jumping back...\n");
        longjmp(jump_buffer, 42); /* 42 = error code; setjmp() in main will return this */
        /* NOTHING after longjmp() ever executes — execution is gone */
    }
    printf("[risky_function] depth=%d  Going deeper...\n", depth);
    risky_function(depth - 1);  /* recurse deeper */
}

int main(void)
{
    /* setjmp() saves the current CPU state into jump_buffer.
     * First call always returns 0 → normal flow.
     * When longjmp(jump_buffer, 42) fires, execution resumes HERE
     * as if setjmp() returned 42 (the value passed to longjmp). */
    int ret = setjmp(jump_buffer);

    if (ret == 0) {
        /* ── Normal path: setjmp returned 0 (first call) ── */
        printf("[main] Calling risky_function(3)...\n");
        risky_function(3);
        printf("[main] This line is never reached.\n");  /* longjmp skips this */

    } else {
        /* ── Recovery path: arrived here via longjmp ──────
         * ret = 42 (the value passed to longjmp).
         * This is equivalent to a 'catch' block in other languages. */
        printf("[main] Caught error code: %d  – recovered successfully!\n", ret);
    }

    return 0;
}
