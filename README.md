# Linux Kernel Internals – Study Notes
> **WSL (Windows Subsystem for Linux)** kernel internals, process management, signals, threads, and system calls — with runnable C examples.

---

## 🚀 Quick Start — Run getpid.c

```bash
# 1. Open WSL terminal  (Win + R → wsl, or Windows Terminal → Ubuntu tab)
cd ~/Linux_Notes

# 2. Compile
gcc getpid.c -o getpid

# 3. Run
./getpid
```

**Expected output:**
```
My  Process ID (PID)  :  1234
My Parent's PID (PPID):  789
```

---

## 📂 Project Structure

```
Linux_Notes/
├── getpid.c                    ← entry point example
├── linux_notes.html            ← 📖 OPEN THIS in Edge/browser — full reference
├── README.md
└── examples/
    ├── 01_process_basics.c     ← fork, PID, PPID, process states
    ├── 02_signals.c            ← SIGKILL, SIGSTOP, SIGCONT, SIGTERM
    ├── 03_daemon.c             ← daemonize() step-by-step
    ├── 04_setjmp_longjmp.c     ← setjmp / longjmp (C try/catch)
    ├── 05_threads_context.c    ← pthreads, mutex, context switching
    ├── 06_system_vs_library.c  ← system(), write() vs printf()
    ├── 07_fopen.c              ← fopen: buffered file I/O
    ├── 08_fork.c               ← fork: parent/child, COW, wait()
    ├── 09_socket.c             ← TCP server: socket/bind/listen/accept
    └── outputs.md              ← 📋 recorded outputs from all programs
```

---

## 🛠️ Compile All Examples

```bash
cd ~/Linux_Notes/examples

# Compile all at once
for f in *.c; do
    gcc "$f" -o "${f%.c}" -lpthread && echo "✓ $f"
done
```

### Run each example
```bash
gcc 07_fopen.c  -o fopen_demo  && ./fopen_demo        # 07  – writes & reads /tmp/fopen_demo.txt
gcc 08_fork.c   -o fork_demo   && ./fork_demo          # 08  – parent + child process
gcc 09_socket.c -o socket_demo && ./socket_demo &      # 09  – TCP server on port 9090
echo "Hello" | nc localhost 9090                       # test the socket server
```

| Flag | Meaning |
|------|---------|
| `-o name` | Output binary name |
| `-Wall` | Enable all warnings |
| `-g` | Include debug symbols |
| `-lpthread` | Link POSIX threads library |

---

## 📋 Essential Commands

### Process Commands

```bash
ps              # processes in current terminal
ps -e           # ALL processes system-wide
ps aux          # extended: user, CPU%, MEM%, command

# ❓ In ps -e, TTY column shows '?'
# '?' means the process has NO controlling terminal → it's a DAEMON
# Examples: systemd, nginx, sshd all show '?'
```

### Job Control

```bash
./my_program &    # run in background  → [1] 4567
jobs              # list background jobs
fg %1             # bring job 1 to foreground
bg %1             # resume stopped job 1 in background
Ctrl+Z            # suspend foreground job (SIGSTOP)
Ctrl+C            # kill foreground job (SIGINT)
```

### Signals / kill

```bash
kill -l           # list all signal names
kill -15 <PID>    # SIGTERM (15) — graceful, catchable
kill -9  <PID>    # SIGKILL (9)  — force kill, CANNOT be caught
kill -19 <PID>    # SIGSTOP (19) — suspend process
kill -18 <PID>    # SIGCONT (18) — resume suspended process
kill %1           # kill by Job ID (not PID)
```

### PID Inspection

```bash
cat /proc/sys/kernel/pid_max   # max PID (usually 4194304)
pstree -p                      # show process hierarchy
dmesg | head                   # kernel boot messages
strace ./getpid                # trace system calls
```

---

## 📖 Topics Covered

Open **`linux_notes.html`** in your browser for the full interactive reference with code examples, tables, and explanations for:

| # | Topic |
|---|-------|
| 1 | PID vs Job ID |
| 2 | Max PID & wrap-around |
| 3 | Process States (R/S/D/T/Z) |
| 4 | Daemon Process |
| 5 | System Boot flow |
| 6 | Turnaround / Response / Throughput |
| 7 | Job Starvation |
| 8 | CPU-bound vs I/O-bound |
| 9 | Signals & kill -9 / -18 / -19 |
| 10 | Suspend State (SIGSTOP) |
| 11 | setjmp / longjmp |
| 12 | Threads & context switching |
| 13 | system() sequential execution |
| 14 | Library vs System Calls |
| 15 | Kernel Space vs User Space |
| 16 | fopen (buffered I/O) |
| 17 | fork() & Copy-on-Write |
| 18 | Sockets (TCP server/client) |

---

## 💡 Open the HTML Reference in Edge

**Option 1 – From WSL terminal:**
```bash
cd ~/Linux_Notes
explorer.exe linux_notes.html     # opens directly in Edge
```

**Option 2 – Edge address bar:**
Type or paste this in Edge's address bar:
```
\\wsl.localhost\Ubuntu\home\ashokatronix\Linux_Notes\linux_notes.html
```

**Option 3 – File Explorer:**
Navigate to `\\wsl.localhost\Ubuntu\home\ashokatronix\Linux_Notes\` and double-click `linux_notes.html`

---

## 🔗 References

- [Linux man pages](https://man7.org/linux/man-pages/) — `man 2 fork`, `man 2 getpid`, etc.
- [The Linux Programming Interface](https://man7.org/tlpi/) — Michael Kerrisk
- `strace ./program` — trace every system call your C program makes
