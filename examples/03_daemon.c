/*
 * 03_daemon.c - Creating a Daemon Process
 *
 * Compile: gcc 03_daemon.c -o daemon_demo
 * Run    : ./daemon_demo
 *          tail -f /tmp/daemon_demo.log   (watch daemon output)
 *          ps -e | grep daemon_demo       (confirm no terminal '?' in TTY col)
 * Ref    : man 7 daemon
 *
 * Daemon Process:
 *   - Runs in background, detached from any terminal (TTY shows '?')
 *   - Parent is usually PID 1 (init/systemd)
 *   - Steps to daemonize:
 *       1. fork() – parent exits, child continues
 *       2. setsid() – child becomes session leader, detaches from terminal
 *       3. fork() again – prevents re-acquiring terminal
 *       4. chdir("/") – avoid blocking mount points
 *       5. Close stdin/stdout/stderr (or redirect to /dev/null)
 *       6. Do your work in a loop
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>

#define LOG_FILE "/tmp/daemon_demo.log"

void daemonize(void)
{
    pid_t pid;

    /* Step 1: First fork */
    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS); /* parent exits */

    /* Step 2: Become session leader */
    if (setsid() < 0) exit(EXIT_FAILURE);

    /* Step 3: Second fork (optional, prevents tty re-attach) */
    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    /* Step 4: Set file permissions mask */
    umask(0);

    /* Step 5: Change working directory */
    chdir("/");

    /* Step 6: Close all open file descriptors */
    for (int fd = sysconf(_SC_OPEN_MAX); fd >= 0; fd--)
        close(fd);

    /* Redirect stdin/stdout/stderr to /dev/null */
    int null_fd = open("/dev/null", O_RDWR);
    dup2(null_fd, STDIN_FILENO);
    dup2(null_fd, STDOUT_FILENO);
    dup2(null_fd, STDERR_FILENO);
}

int main(void)
{
    daemonize();

    FILE *log = fopen(LOG_FILE, "w");
    if (!log) return 1;

    fprintf(log, "Daemon started. PID=%d\n", getpid());
    fflush(log);

    int count = 0;
    while (count < 10) {
        time_t now = time(NULL);
        fprintf(log, "[tick %d] %s", count++, ctime(&now));
        fflush(log);
        sleep(2);
    }

    fprintf(log, "Daemon exiting.\n");
    fclose(log);
    return 0;
}
