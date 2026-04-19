/*
 * 05_threads_context.c - POSIX Threads & Context Switching
 *
 * Compile: gcc 05_threads_context.c -o threads -lpthread
 * Run    : ./threads
 * Ref    : man 7 pthreads, man 3 pthread_create
 *
 * Thread vs Process:
 *   - Threads share the SAME address space (heap, global vars, code).
 *   - Context switch between threads is CHEAPER than between processes
 *     because there is NO memory-map switch (no TLB flush needed).
 *   - The kernel saves/restores only: CPU registers, stack pointer, PC.
 *   - Threads within one process share file descriptors, signal handlers,
 *     and memory – so synchronization (mutex/semaphore) is required.
 *
 * CPU-bound vs I/O-bound:
 *   CPU-bound: Most time spent computing. Threads help on multi-core.
 *   I/O-bound : Most time waiting for disk/network. Even one core benefits
 *               from threads because one thread can wait while another runs.
 */

#include <stdio.h>
#include <pthread.h>   /* pthread_create, pthread_join, pthread_mutex_t */
#include <unistd.h>

#define NUM_THREADS 4

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;  /* lock: only 1 thread at a time */
int shared_counter = 0;  /* shared by ALL threads – lives in common heap/data segment */

void *thread_fn(void *arg)
{
    int id = *(int *)arg;  /* cast void* back to int* to get thread id (0,1,2,3) */

    for (int i = 0; i < 5; i++) {
        /* --- CRITICAL SECTION START ---
         * Without this lock, two threads could both read counter=5,
         * both increment to 6, both write 6 → one increment lost (race condition).
         * Lock makes read→increment→write ATOMIC (uninterruptible). */
        pthread_mutex_lock(&lock);     /* 🔒 acquire lock – others block here */
        shared_counter++;              /* safe: only one thread runs this at a time */
        printf("[Thread %d] counter = %d\n", id, shared_counter);
        pthread_mutex_unlock(&lock);   /* 🔓 release lock – next waiting thread unblocks */
        /* --- CRITICAL SECTION END --- */

        usleep(100000); /* sleep 100ms – simulates I/O wait, triggers context switch
                         * while sleeping, OS runs another thread (cooperative overlap) */
    }
    return NULL;  /* thread exits; pthread_join() in main will collect this */
}

int main(void)
{
    pthread_t tids[NUM_THREADS];  /* stores thread handles (used by pthread_join) */
    int ids[NUM_THREADS];         /* per-thread id values passed as argument */

    /* TID is a large number – it's the memory address of the thread's internal struct */
    printf("Main thread PID=%d  TID=%lu\n", getpid(), pthread_self());

    /* Spawn all threads – OS may schedule them in ANY order */
    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;
        /* pthread_create(handle, attr, function, arg)
         * Each thread starts executing thread_fn(&ids[i]) independently */
        pthread_create(&tids[i], NULL, thread_fn, &ids[i]);
    }

    /* Wait for every thread to finish before continuing.
     * Without join, main() could exit and kill all threads prematurely. */
    for (int i = 0; i < NUM_THREADS; i++)
        pthread_join(tids[i], NULL);

    /* If mutex worked correctly: 4 threads × 5 increments = always 20 */
    printf("Final counter = %d  (expected %d)\n",
           shared_counter, NUM_THREADS * 5);

    pthread_mutex_destroy(&lock);  /* free mutex resources */
    return 0;
}
