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

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int shared_counter = 0;

void *thread_fn(void *arg)
{
    int id = *(int *)arg;

    for (int i = 0; i < 5; i++) {
        /* Lock protects shared_counter from race conditions */
        pthread_mutex_lock(&lock);
        shared_counter++;
        printf("[Thread %d] counter = %d\n", id, shared_counter);
        pthread_mutex_unlock(&lock);
        usleep(100000); /* 100 ms – simulate I/O wait */
    }
    return NULL;
}

int main(void)
{
    pthread_t tids[NUM_THREADS];
    int ids[NUM_THREADS];

    printf("Main thread PID=%d  TID=%lu\n", getpid(), pthread_self());

    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;
        pthread_create(&tids[i], NULL, thread_fn, &ids[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++)
        pthread_join(tids[i], NULL);  /* wait for each thread */

    printf("Final counter = %d  (expected %d)\n",
           shared_counter, NUM_THREADS * 5);

    pthread_mutex_destroy(&lock);
    return 0;
}
