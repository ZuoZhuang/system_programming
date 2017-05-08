/**
 * Machine Problem: Password Cracker
 * CS 241 - Spring 2017
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <crypt.h>

#include "cracker1.h"
#include "libs/format.h"
#include "libs/utils.h"
#include <sys/types.h>
#include "libs/queue.h"

/*#define DEBUG 1*/

int num_recovered = 0;
int num_failed = 0;
pthread_mutex_t m;
queue *work_queue;
int queue_len = 0;

void *crack_thread(void *pid) {
  char *ori_str = NULL;

#ifdef DEBUG
  /*fprintf(stderr, "%d\n", *((int *)pid));*/
#endif

    while (1) {
        // TODO cracking
        // Lock and get element from queue, if there's none, stop the thread
        pthread_mutex_lock(&m);
        if (queue_len == 0) {
            pthread_mutex_unlock(&m);
            return NULL;
        }
        ori_str = queue_pull(work_queue);
        queue_len--;
        pthread_mutex_unlock(&m);

        if (ori_str == NULL) {
            exit(-1);
        }

#ifdef DEBUG
        /*fprintf(stderr, "%s\n", ori_str);*/
#endif

        // parse input
        char *username = strtok(ori_str, " ");
        if (username == NULL) {
            exit(-1);
        }

        char *target_hash = strtok(NULL, " ");
        if (target_hash == NULL) {
            exit(-1);
        }

        char *part_password = strtok(NULL, " ");
        if (part_password == NULL) {
            exit(-1);
        }

#ifdef DEBUG
        fprintf(stderr, "%s, %s, %s\n", username, target_hash, part_password);
#endif

      // start the cracking
        v1_print_thread_start(*((int *)pid), username);

        char *password = (char *) malloc(sizeof(char) * strlen(part_password) + 1);
        strcpy(password, part_password);
        password[strlen(part_password)] = '\0';

#ifdef DEBUG
        /*fprintf(stderr, "%s, %d\n", password, (int)strlen(password));*/
#endif

        char *try_pass = password + getPrefixLength(password);

        int hashCount = 0;
        int result = 1;

        setStringPosition(try_pass, 0);

        struct crypt_data cdata;
        cdata.initialized = 0;
        if (strcmp(crypt_r(password, "xx", &cdata), target_hash) == 0) {
            result = 0;
        }
        hashCount++;

        while (result == 1 && incrementString(try_pass) != 0) {
            hashCount++;
            if (strcmp(crypt_r(password, "xx", &cdata), target_hash) == 0) {
                result = 0;
                break;
            }
        }

        if (result == 0) {
            pthread_mutex_lock(&m);
            num_recovered++;
            pthread_mutex_unlock(&m);
        } else {
            pthread_mutex_lock(&m);
            num_failed++;
            pthread_mutex_unlock(&m);
        }

#ifdef DEBUG
        /*fprintf(stderr, "%s, %d\n", try_pass, (int)strlen(try_pass));*/
#endif

        v1_print_thread_result(*((int *)pid), username, password, hashCount, getThreadCPUTime(), result);

        // Clean up the variables
        free(password);
        password = NULL;
        try_pass = NULL;
        hashCount = 0;
        result = 0;
    }


  return NULL;
}

int start(size_t thread_count) {
  // TODO your code here, make sure to use thread_count!
  // Remember to ONLY crack passwords in other threads

  pthread_t tid[thread_count];
  void *result;

  char *line = NULL;
  size_t num = 0;

  // Initialize the queue
  work_queue = queue_create(-1, NULL, NULL);

  // Put element in the queue
  while (getline(&line, &num, stdin) != -1) {

#ifdef DEBUG
  /*fprintf(stderr, "%s\n", line);*/
#endif

    int len = strlen(line);
    if (line[len-1] == '\n') {
      line[len-1] = '\0';
    }
    queue_push(work_queue, (char *)strdup(line));
    queue_len++;
    free(line);
    line = NULL;
    num = 0;
  }
  queue_push(work_queue, "\0");

  // Start threads
  /*double startTime = getTime();*/
  int *ids = (int *)malloc(sizeof(int) * thread_count);
  for (unsigned int i = 0; i < thread_count; i++) {
      ids[i] = i+1;
      pthread_create(&tid[i], NULL, crack_thread, (ids+i));
  }

  for (unsigned int i = 0; i < thread_count; i++) {
      pthread_join(tid[i], &result);
  }
  // Print summary
  v1_print_summary(num_recovered, num_failed);

  // Clean up
  queue_destroy(work_queue);
  pthread_mutex_destroy(&m);
  free(ids);

  return 0;
}
