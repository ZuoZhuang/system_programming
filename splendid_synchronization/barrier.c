/**
 * Splendid Synchronization Lab
 * CS 241 - Spring 2017
 */
#include "barrier.h"
#include <stdlib.h>

// The returns are just for errors if you want to check for them.
int barrier_destroy(barrier_t *barrier) {
    if (barrier == NULL) return -1;

    int error = 0;

    error = pthread_mutex_destroy(&(barrier->mtx));
    error = pthread_cond_destroy(&(barrier->cv));

  return error;
}

int barrier_init(barrier_t *barrier, unsigned int num_threads) {
    if (barrier == NULL) return -1;

    int error = 0;

    error = pthread_mutex_init(&(barrier->mtx), NULL);
    error = pthread_cond_init(&(barrier->cv), NULL);

    barrier->n_threads = num_threads;
    barrier->count = 0;
    barrier->times_used = 0;

  return error;
}

int barrier_wait(barrier_t *barrier) {
    pthread_mutex_lock(&(barrier->mtx));
    barrier->count++;

    unsigned int temp = barrier->times_used;

    if (barrier->count == barrier->n_threads) {
        barrier->times_used++;
        barrier->count = 0;
        pthread_cond_broadcast(&(barrier->cv));
    }
    else {
        while (temp == barrier->times_used) pthread_cond_wait(&(barrier->cv), &(barrier->mtx));
    }

    pthread_mutex_unlock(&(barrier->mtx));

    return 0;
}
