/*
 * 02_signals.c - Signal handling: SIGSTOP(19), SIGCONT(18), SIGKILL(9)
 *
 * Compile: gcc 02_signals.c -o signals
 * Run    : ./signals &        (run in background, note PID printed)
 *          kill -19 <PID>    (suspend / SIGSTOP)
 *          kill -18 <PID>    (resume  / SIGCONT)
 *          kill -9  <PID>    (force kill / SIGKILL)
 *          kill -l           (list all signal numbers)
 * Ref    : man 7 signal
 *
 * Key Signals:
 *   SIGKILL (9)  – Cannot be caught/ignored. Immediate death.
 *   SIGTERM (15) – Graceful termination request (can be caught).
 *   SIGSTOP (19) – Suspend process (cannot be caught).
 *   SIGCONT (18) – Resume a stopped process.
 *   SIGINT  (2)  – Ctrl+C interrupt.
 *   SIGHUP  (1)  – Hangup (terminal closed).
 */

#include <stdio.h>
#include <signal.h>
#include <unistd.h>

/* Custom handler – called ONLY when SIGTERM (15) arrives.
 * SIGKILL (9) and SIGSTOP (19) CANNOT be caught — the kernel handles them directly.
 * Use _exit() not exit() inside signal handlers to avoid re-entrancy issues. */
void handle_sigterm(int sig)
{
    printf("\n[Signal] Caught SIGTERM (%d) – cleaning up gracefully...\n", sig);
    /* do cleanup here (close files, free resources, etc.) */
    _exit(0);  /* _exit() skips atexit() handlers and stdio flushing (safer in handlers) */
}

int main(void)
{
    /* Register SIGTERM handler.
     * signal(signum, handler) — when signum arrives, call handler instead of default action.
     * Default action for SIGTERM = terminate. Here we override it with our cleanup. */
    signal(SIGTERM, handle_sigterm);

    /* Print our own PID so you can target us with kill from another terminal */
    printf("PID=%d  Waiting for signals. Try:\n", getpid());
    printf("  kill -15 %d   (graceful → handler runs)\n", getpid());
    printf("  kill -19 %d   (suspend  → cannot catch, OS freezes us)\n", getpid());
    printf("  kill -18 %d   (resume   → cannot catch, OS unfreezes us)\n", getpid());
    printf("  kill -9  %d   (force kill → cannot catch, instant death)\n", getpid());

    /* Infinite loop – process stays alive, printing every 2s.
     * tick... stops when: SIGSTOP (suspended), SIGKILL (dead), or SIGTERM (handler exits).
     * If SIGSTOP is sent → tick pauses. SIGCONT → tick resumes from where it stopped. */
    while (1) {
        printf("[running] tick...\n");
        sleep(2);  /* sleep puts process in 'S' (interruptible sleep) state */
    }

    return 0;
}
