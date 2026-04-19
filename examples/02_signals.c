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

/* Custom handler – called when SIGTERM (15) arrives */
void handle_sigterm(int sig)
{
    printf("\n[Signal] Caught SIGTERM (%d) – cleaning up gracefully...\n", sig);
    /* do cleanup here */
    _exit(0);
}

int main(void)
{
    /* Register SIGTERM handler */
    signal(SIGTERM, handle_sigterm);

    printf("PID=%d  Waiting for signals. Try:\n", getpid());
    printf("  kill -15 %d   (graceful)\n", getpid());
    printf("  kill -19 %d   (suspend)\n",  getpid());
    printf("  kill -18 %d   (resume)\n",   getpid());
    printf("  kill -9  %d   (force kill)\n",getpid());

    /* Infinite loop – keeps process alive */
    while (1) {
        printf("[running] tick...\n");
        sleep(2);
    }

    return 0;
}
