/**
 * Splendid Synchronization Lab
 * CS 241 - Spring 2017
 */
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "queue.h"


void *thread_func_throwaway(void *var) {
    queue *myqueue = (queue *)var;
      int i = 0;
      while(i < 10) {queue_push(myqueue, "a"); i++;}
  return (void *)NULL;
}

void *thread_func_reusable(void *var) {
    queue *myqueue = (queue *)var;
    int j = 0;
    while(j < 10) {printf("%s",(char *) queue_pull(myqueue));j++;}
  return (void *)NULL;
}


int main(int argc, char **argv) {

  queue * myqueue = queue_create(3, char_copy_constructor, char_destructor);

  /*int a = 1;*/
  /*queue_push(myqueue, (void *)&a);*/

  /*int *a = (int *)queue_pull(myqueue);*/

    pthread_t thread1;
    pthread_t thread2;

    pthread_create(&thread1, NULL, &thread_func_throwaway, (void *)myqueue);
    pthread_create(&thread2, NULL, &thread_func_reusable, (void *)myqueue);

    /*int status[2];*/
    /*int status2;*/

    /*pthread_join(thread1, (void **)&status[0]);*/
    /*pthread_join(thread2, (void **)&status[1]);*/

    sleep(10);

  printf("Please write tests cases\n");
  return 0;
}
