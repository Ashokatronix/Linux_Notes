# C Program Outputs

Collected outputs from all example programs in `Linux_Notes/examples/`.

---

## 01 — Process Basics (`01_process_basics.c`)

> **Concepts:** `fork()`, PID, PPID, `wait()`

```
=== Process Basics ===
[Parent] PID=8759  PPID=7060
[Parent] Spawned child PID=8760
[Child ] PID=8760  PPID=8759
[Child ] Done. Exiting.
[Parent] Child finished. Parent exiting.
```

---

## 02 — Signals (`02_signals.c`)

> **Concepts:** `signal()`, `sigaction()`, SIGINT, SIGTERM, SIGUSR1

### Run 1 — SIGTERM (kill -15): Graceful exit
```
PID=10142  Waiting for signals. Try:
  kill -15 10142   (graceful)
  kill -19 10142   (suspend)
  kill -18 10142   (resume)
  kill -9  10142   (force kill)
[running] tick...   ← (×49 ticks, ~98 seconds)

[Signal] Caught SIGTERM (15) – cleaning up gracefully...
```
> Sent from another terminal: `sudo kill -15 10142`
> Custom `handle_sigterm()` handler executed → clean exit.

---

### Run 2 — SIGSTOP (kill -19): Suspend
```
PID=10158  Waiting for signals. Try: ...
[running] tick...   ← (×14 ticks)

[1]+  Stopped                 ./out
```
> Sent: `kill -19 10158` → process **frozen** by the OS.
> `[1]+ Stopped` is the shell reporting the job was suspended.

---

### Run 3 — SIGCONT + SIGSTOP toggle, then SIGKILL (kill -9)
```
PID=10162  Waiting for signals. Try: ...
[running] tick...   ← (×14 ticks)

[2]+  Stopped                 ./out
```
> Sequence from other terminal:
> `kill -18 10162` (resume) → `kill -19 10162` (suspend) → `kill -18 10162` (resume) → `kill -19 10162` (suspend)
> tick... output flooded the prompt since process was in background.
> Finally: `kill -18 10162` then `kill -9 10162` → force killed.

---

### Run 4 — SIGKILL (kill -9): Force kill
```
PID=10166  Waiting for signals. Try: ...
[running] tick...   ← (×5 ticks)
Killed
```
> Sent: `kill -9 10166` → OS killed instantly, no handler ran.
> Shell prints just `Killed` — no cleanup possible.

---

## 03 — Daemon (`03_daemon.c`)

> **Concepts:** `fork()`, `setsid()`, daemonization

```
$ ./out
$                          ← shell returns immediately (parent exited, daemon detached)

$ tail -f /tmp/daemon_demo.log
Daemon started. PID=11303
[tick 0] Sun Apr 19 07:46:38 2026
[tick 1] Sun Apr 19 07:46:40 2026
[tick 2] Sun Apr 19 07:46:42 2026
[tick 3] Sun Apr 19 07:46:44 2026
[tick 4] Sun Apr 19 07:46:46 2026
[tick 5] Sun Apr 19 07:46:49 2026
[tick 6] Sun Apr 19 07:46:51 2026
[tick 7] Sun Apr 19 07:46:53 2026
[tick 8] Sun Apr 19 07:46:55 2026
[tick 9] Sun Apr 19 07:46:57 2026
Daemon exiting.
```

> **Note:** `tick 0` was missed because `tail -f` was started after the first tick.
> Daemon runs for ~20 seconds (10 ticks × 2s), writes to `/tmp/daemon_demo.log`, exits cleanly.
> Confirmed running via `ps -e`: TTY column shows `?` (no terminal) = true daemon.

---

## 04 — setjmp / longjmp (`04_setjmp_longjmp.c`)

> **Concepts:** non-local jumps, `setjmp()`, `longjmp()`, error recovery

```
[main] Calling risky_function(3)...
[risky_function] depth=3  Going deeper...
[risky_function] depth=2  Going deeper...
[risky_function] depth=1  Going deeper...
[risky_function] Encountered an error! Jumping back...
[main] Caught error code: 42  – recovered successfully!
```

> `longjmp(jump_buffer, 42)` teleports execution back to `setjmp()` in main,
> which then returns 42 → `else` branch runs → "recovered successfully!"

---

## 05 — Threads & Context (`05_threads_context.c`)

> **Concepts:** `pthread_create()`, `pthread_join()`, mutex, shared counter

```
Main thread PID=13248  TID=140670714906432
[Thread 1] counter = 1
[Thread 2] counter = 2
[Thread 3] counter = 3
[Thread 0] counter = 4
[Thread 1] counter = 5
[Thread 2] counter = 6
[Thread 0] counter = 7
[Thread 3] counter = 8
[Thread 1] counter = 9
[Thread 2] counter = 10
[Thread 0] counter = 11
[Thread 3] counter = 12
[Thread 1] counter = 13
[Thread 2] counter = 14
[Thread 0] counter = 15
[Thread 3] counter = 16
[Thread 1] counter = 17
[Thread 2] counter = 18
[Thread 3] counter = 19
[Thread 0] counter = 20
Final counter = 20  (expected 20)
```

> 4 threads share one counter. Order varies each run (OS scheduler decides).
> Mutex ensures no two threads increment simultaneously → final result always = 20.

---

## 06 — System vs Library Calls (`06_system_vs_library.c`)

> **Concepts:** `printf()` vs `write()`, user space vs kernel space, `system()`

```
Hello from printf() [library call - user space]
Hello from write() [system call - kernel space]

--- system() examples (sequential) ---
system(): running ls -l /tmp
total 28
-rw-rw-rw- 1 ashokatronix ashokatronix  382 Apr 19 07:53 daemon_demo.log
drwx------ 2 root         root         4096 Apr 19 04:03 snap-private-tmp
drwx------ 3 root         root         4096 Apr 19 04:05 systemd-private-...polkit.service-DgOtYK
drwx------ 3 root         root         4096 Apr 19 04:03 systemd-private-...systemd-logind.service-UMvxU3

--- Done ---
```

> `printf()` → buffered library call (user space) → eventually calls `write()` internally.
> `write()` → direct system call → crosses into kernel space immediately.
> `system("ls -l /tmp")` → forks a shell, runs command, waits for it to finish.

---

## 07a — File I/O with fopen (`07a_fopen.c`)

> **Concepts:** `fopen()`, `fprintf()`, `fgets()`, buffered file I/O, `/tmp` files

```
[fopen] Written to /tmp/fopen_demo.txt
[fopen] Reading back:
  Line 1: Hello from fopen!
  Line 2: PID of writer = implicit (this process)
  Line 3: fopen uses buffered I/O
```

**File contents of `/tmp/fopen_demo.txt`:**
```
Line 1: Hello from fopen!
Line 2: PID of writer = implicit (this process)
Line 3: fopen uses buffered I/O
```

> `fopen()` is a buffered library call (user space).
> Data is written to a FILE* buffer first, flushed to disk on `fclose()` or `fflush()`.
> File persists in `/tmp` and can be read back with `tail -f` or `cat`.

---

## 07b — Fork Deep Dive (`08_fork.c`)

> **Concepts:** `fork()`, PID/PPID, `wait()`, `WIFEXITED()`, Copy-on-Write

```
[before fork] PID=15291
[PARENT] PID=15291  spawned child PID=15292
[CHILD ] PID=15292  PPID=15291
[CHILD ] Doing some work...
[CHILD ] Done. Exiting.
[PARENT] Child exited with code 0
```

> Parent and child run the same code after `fork()` — return value decides who is who.
> `wait()` blocks parent until child exits → prevents zombie process.
> Child exit code `0` collected via `WIFEXITED` + `WEXITSTATUS`.

---

## 07c — Socket Basics (`09_socket.c`)

> **Concepts:** `socket()`, `bind()`, `listen()`, `accept()`, `connect()`, TCP

**Terminal 1 — Server:**
```
[server] Listening on port 9090  (try: echo Hi | nc localhost 9090)
[server] Client connected from 127.0.0.1:43278
[server] Received 19 bytes: Hello from client!
[server] Done.
```

**Terminal 2 — Client (`nc`):**
```
$ echo "Hello from client!" | nc localhost 9090
Hello from server! Goodbye.
```

> Server binds to port 9090, waits with `accept()`, receives message, replies, closes.
> Client connects via `nc` (netcat), sends string, receives server reply, disconnects.
> `127.0.0.1` = loopback (localhost). Port `43278` = ephemeral port assigned by OS to client.

---

*Add outputs above as you run each program.*
*Use `./program | tee -a outputs.md` or paste manually.*
