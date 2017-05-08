/**
 * Teaching Threads
 * CS 241 - Spring 2017
 */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "reduce.h"
#include "reducers.h"

// Task struct that gets passed into pthread_create
typedef struct {
  int *output;         // Pointer to where the output should be written
  int *list;           // Pointer to the begining of the list.
  size_t list_len;     // Length of the list.
  reducer reduce_func; // Reduce function for reduce()
  int base_case;       // Base case for the reduce function
} task;

/* You should create a start routine for your threads. */

void *thread_launcher(void *in_work){
    task *work = (task *)in_work;
    *(work->output) = reduce(work->list, work->list_len, work->reduce_func, work->base_case);

    return NULL;
}

int par_reduce(int *list, size_t list_len, reducer reduce_func, int base_case,
               size_t num_threads) {
  /* Your implementation goes here */
    num_threads = num_threads <= list_len ? num_threads : list_len;
    int *output = (int *) malloc(sizeof(int)*num_threads);
    size_t remain = list_len % num_threads;
    size_t step = list_len / num_threads + (remain == 0 ? 0 : 1);

    /*num_threads = num_threads <= list_len ? num_threads : list_len;*/

    task **task_list = (task **) malloc(sizeof(task *)*num_threads);


    // heap imp
    /*pthread_t **thread_arr = (pthread_t **) malloc(sizeof(pthread_t *)*num_threads);*/

    // stack imp
    pthread_t thread_arr[num_threads];

    size_t i = 0;

    for (i = 0; i < num_threads-1; i++) {
        task_list[i] = (task *) malloc(sizeof(task));
        task_list[i]->output = output + i;
        task_list[i]->list = list + i * step;
        task_list[i]->list_len = step;
        task_list[i]->reduce_func = reduce_func;
        task_list[i]->base_case = base_case;

        // Stack imp
        /*if (pthread_create(&(thread_arr[i]), NULL, (void *)reduce, (void *)task_list[i]) != 0) {*/
            /*printf("create failed");*/
            /*return 1;*/
        /*}*/

        if (pthread_create(&(thread_arr[i]), NULL, (void *)thread_launcher, (void *)task_list[i]) != 0) {
            printf("create failed");
            return 1;
        }

        // Heap imp
        /*if (pthread_create(thread_arr[i], NULL, (void *)reduce, (void *)task_list[i]) != 0) {*/
            /*printf("create failed");*/
            /*return 1;*/
        /*}*/
    }

    task_list[i] = (task *) malloc(sizeof(task));
    task_list[i]->output = output + i;
    task_list[i]->list = list + i * step;
    task_list[i]->list_len = (remain == 0 ? step : list_len % step);
    /*task_list[i]->list_len = (size_t) remain == 0 ? step : remain;*/
    task_list[i]->reduce_func = reduce_func;
    task_list[i]->base_case = base_case;

    // Stack imp
    /*if (pthread_create(&(thread_arr[i]), NULL, (void *)reduce, (void *)task_list[i]) != 0) {*/
        /*printf("create failed");*/
        /*return 1;*/
    /*}*/

    if (pthread_create(&(thread_arr[i]), NULL, (void *)thread_launcher, (void *)task_list[i]) != 0) {
        printf("create failed");
        return 1;
    }

    // Heap imp
    /*if (pthread_create(thread_arr[i], NULL, (void *)reduce, (void *)task_list[i]) != 0) {*/
        /*printf("create failed");*/
        /*return 1;*/
    /*}*/

    void *result;

    for (size_t j = 0; j <num_threads; j++) {
        /*printf("output of therad %d: %d\n", (int)j, *(task_list[j]->output));*/
        pthread_join(thread_arr[j], &result);
    }

/*int reduce(int *list, size_t length, reducer reduce_func, int base_case);*/
    int res = reduce(output, num_threads, reduce_func, base_case);

    for (size_t j = 0; j < num_threads; j++) {
        free(task_list[j]);
    }

    free(task_list);
    free(output);

    /*printf("res: %d\n", res);*/
    return res;
}
