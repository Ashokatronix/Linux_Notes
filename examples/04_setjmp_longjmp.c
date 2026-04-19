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

static jmp_buf jump_buffer;  /* saves the CPU state */

void risky_function(int depth)
{
    if (depth == 0) {
        printf("[risky_function] Encountered an error! Jumping back...\n");
        longjmp(jump_buffer, 42); /* jump back; setjmp returns 42 */
    }
    printf("[risky_function] depth=%d  Going deeper...\n", depth);
    risky_function(depth - 1);
}

int main(void)
{
    int ret = setjmp(jump_buffer); /* save state here */

    if (ret == 0) {
        /* First time: normal flow */
        printf("[main] Calling risky_function(3)...\n");
        risky_function(3);
        printf("[main] This line is never reached.\n");
    } else {
        /* Arrived here via longjmp */
        printf("[main] Caught error code: %d  – recovered successfully!\n", ret);
    }

    return 0;
}
