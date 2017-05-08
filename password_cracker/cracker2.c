/**
 * Machine Problem: Password Cracker
 * CS 241 - Spring 2017
 */


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <crypt.h>

// for sleep
#include <unistd.h>

#include "cracker2.h"
#include "libs/format.h"
#include "libs/utils.h"
#include <sys/types.h>
#include "libs/queue.h"

/*#define DEBUG 1*/

typedef struct _crack_task {
    char *username;
    char *target_hash;
    char *password;
    int result;
    int thread_count;
    int hashCount;
} crack_task;

pthread_mutex_t m;
queue *work_queue;
int queue_len = 0;
crack_task *curr_task;

void task_init(crack_task *this,
               char *username,
               char *target_hash,
               char *password,
               int thread_count) {

    this->username = (char *) malloc(sizeof(char) * strlen(username) + 1);
    strcpy(this->username, username);
    this->target_hash = (char *) malloc(sizeof(char) * strlen(target_hash) + 1);
    strcpy(this->target_hash, target_hash);
    this->password = (char *) malloc(sizeof(char) * strlen(password) + 1);
    strcpy(this->password, password);
    this->result = 2;
    this->thread_count = thread_count;
    this->hashCount = 0;
}

void task_destroy(crack_task *this) {
    free(this->username);
    free(this->target_hash);
    free(this->password);
    free(this);
}

void *crack_thread(void *tid) {
#ifdef DEBUG
    fprintf(stderr, "%s, %s, %s\n", curr_task->username, curr_task->target_hash, curr_task->password);
#endif

    // prepare for cracking
    pthread_mutex_lock(&m);
    size_t password_len = strlen(curr_task->password);
    char *password = (char *) malloc(sizeof(char) * password_len + 1);
    strcpy(password, curr_task->password);
    pthread_mutex_unlock(&m);
    password[strlen(curr_task->password)] = '\0';

    size_t prefix_len = getPrefixLength(password);
    char *try_pass = password + prefix_len;

    // Calculate start and end offset
    long start_index;
    long count;
    getSubrange(password_len-prefix_len, curr_task->thread_count, *((int *)tid),
                 &start_index, &count);

    setStringPosition(try_pass, start_index);

    // start the cracking
    v2_print_thread_start(*((int *)tid), curr_task->username, start_index,
                           password);

    int hashCount = 0;
    int result = 2;

    struct crypt_data cdata;
    cdata.initialized = 0;
    if (strcmp(crypt_r(password, "xx", &cdata), curr_task->target_hash) == 0) {
        result = 0;
    }
    hashCount++;

    while (result == 2 && hashCount != count && incrementString(try_pass) != 0) {
        hashCount++;
        if (strcmp(crypt_r(password, "xx", &cdata), curr_task->target_hash) == 0) {
            result = 0;
            break;
        }

        // check whether the password is cracked by other threads
        pthread_mutex_lock(&m);
        if (curr_task->result == 0) {
            pthread_mutex_unlock(&m);
            result = 1;
            break;
        }
        pthread_mutex_unlock(&m);
    }

    pthread_mutex_lock(&m);
    if (result == 0) {
        strcpy(curr_task->password, password);
        curr_task->result = 0;
    }
    curr_task->hashCount += hashCount;
    pthread_mutex_unlock(&m);

    v2_print_thread_result(*((int *)tid), hashCount, result);

    // Clean up the variables
    free(password);
    password = NULL;
    try_pass = NULL;
    hashCount = 0;
    result = 0;

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

    // Creat multiple thread for every task
    while (queue_len != 0) {
        char *ori_str = queue_pull(work_queue);
        queue_len--;

        // Parse string and save parameters into struct

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
    /*return 0;*/
#endif

        // Store information in task struct
        curr_task = (crack_task *) malloc(sizeof(crack_task));
        task_init(curr_task, username, target_hash, part_password, thread_count);

        // Print thread start message
        v2_print_start_user(curr_task->username);

        // Get Start times
        int start = getTime();
        int CPU_start = getCPUTime();

        // Start threads
        int ids[thread_count];
        for (unsigned int i = 0; i < thread_count; i++) {
            ids[i] = i+1;
            pthread_create(&tid[i], NULL, crack_thread, &(ids[i]));
        }

        for (unsigned int i = 0; i < thread_count; i++) {
          pthread_join(tid[i], &result);
        }

        // Print Summary
        v2_print_summary(curr_task->username, curr_task->password, curr_task->hashCount,
                         getTime()-start, getCPUTime()-CPU_start, curr_task->result);

        // Clean up after each epoch
        task_destroy(curr_task);
    }

    // Clean up
    queue_destroy(work_queue);
    pthread_mutex_destroy(&m);

    return 0;
}

