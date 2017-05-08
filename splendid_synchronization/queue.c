/**
 * Splendid Synchronization Lab
 * CS 241 - Spring 2017
 */
#include "queue.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

/**
 * This queue is implemented with a linked list of queue_node's.
 */
typedef struct queue_node {
  void *data;
  struct queue_node *next;
} queue_node;

struct queue {
  /* The function callback for the user to define the way they want to copy
   * elements */
  copy_constructor_type copy_constructor;

  /* The function callback for the user to define the way they want to destroy
   * elements */
  destructor_type destructor;

  /* queue_node pointers to the head and tail of the queue */
  queue_node *head, *tail;

  /* The number of elements in the queue */
  ssize_t size;

  /**
   * The maximum number of elements the queue can hold.
   * max_size is non-positive if the queue does not have a max size.
   */
  ssize_t max_size;

  /* Mutex and Condition Variable for thread-safety */
  pthread_cond_t cv;
  pthread_mutex_t m;
};

queue *queue_create(ssize_t max_size, copy_constructor_type copy_constructor,
                    destructor_type destructor) {
  /* Your code here */
    queue *myqueue = (queue *) malloc(sizeof(queue));

    myqueue->copy_constructor = copy_constructor;
    myqueue->destructor = destructor;
    myqueue->head = NULL;
    myqueue->tail = NULL;
    myqueue->size = 0;
    myqueue->max_size = max_size;

    pthread_mutex_init(&(myqueue->m), NULL);
    pthread_cond_init(&(myqueue->cv), NULL);

  return myqueue;
}

void queue_destroy(queue *this) {
    free(this->head);
    free(this->tail);
    pthread_mutex_destroy(&(this->m));
    pthread_cond_destroy(&(this->cv));
}

void queue_push(queue *this, void *data) {
    pthread_mutex_lock(&(this->m));

    if (this->max_size>0) {
        while (this->size == this->max_size) pthread_cond_wait(&(this->cv), &(this->m));
    }

    queue_node *new_node = (queue_node *) malloc(sizeof(queue_node));
    new_node->data = data;

    if (this->tail != NULL) new_node->next = (this->tail)->next;
    else new_node->next = NULL;

    this->tail = new_node;

    this->size++;

    pthread_cond_signal(&(this->cv));

    pthread_mutex_unlock(&(this->m));
}

void *queue_pull(queue *this) {
    queue_node *ret = NULL;

    pthread_mutex_lock(&(this->m));

    while (this->size <= 0) pthread_cond_wait(&(this->cv), &(this->m));

    if (this->head != NULL) ret = this->head;
    /*if (ret != NULL) this->head = ret->next;*/

    /*this->size--;*/

    /*pthread_cond_signal(&(this->cv));*/

    /*pthread_mutex_unlock(&(this->m));*/

    return (void *)ret->data;
}


// The following is code generated:

queue *char_queue_create() {
  return queue_create(-1, char_copy_constructor, char_destructor);
}
queue *double_queue_create() {
  return queue_create(-1, double_copy_constructor, double_destructor);
}
queue *float_queue_create() {
  return queue_create(-1, float_copy_constructor, float_destructor);
}
queue *int_queue_create() {
  return queue_create(-1, int_copy_constructor, int_destructor);
}
queue *long_queue_create() {
  return queue_create(-1, long_copy_constructor, long_destructor);
}
queue *short_queue_create() {
  return queue_create(-1, short_copy_constructor, short_destructor);
}
queue *unsigned_char_queue_create() {
  return queue_create(-1, unsigned_char_copy_constructor,
                      unsigned_char_destructor);
}
queue *unsigned_int_queue_create() {
  return queue_create(-1, unsigned_int_copy_constructor,
                      unsigned_int_destructor);
}
queue *unsigned_long_queue_create() {
  return queue_create(-1, unsigned_long_copy_constructor,
                      unsigned_long_destructor);
}
queue *unsigned_short_queue_create() {
  return queue_create(-1, unsigned_short_copy_constructor,
                      unsigned_short_destructor);
}
