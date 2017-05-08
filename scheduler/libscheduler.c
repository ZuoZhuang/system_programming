/**
 * Scheduler Lab
 * CS 241 - Spring 2017
 */
#include "libpriqueue/libpriqueue.h"
#include "libscheduler.h"

static priqueue_t pqueue;
static core_t core;
static scheme_t scheme;
static int (*comparison_func)(const void *, const void *);

static int num_served;
static int total_waiting_time;
static int total_turnaround_time;
static int total_response_time;

#define DEBUG 1

int comparer_fcfs(const void *a, const void *b) {
    job_t *job_a = (job_t *)a;
    job_t *job_b = (job_t *)b;
    job_a->priority = job_a->arrival_time;
    job_b->priority = job_b->arrival_time;

    return job_a->arrival_time - job_b->arrival_time;
    /*return 0;*/
}

int break_tie(const void *a, const void *b) {
    // TODO
    return comparer_fcfs(a, b);
}

int comparer_ppri(const void *a, const void *b) {
    // TODO
    // Complete as is
    return comparer_pri(a, b);
}

int comparer_pri(const void *a, const void *b) {
    job_t *job_a = (job_t *)a;
    job_t *job_b = (job_t *)b;
    int pri_diff = job_a->priority - job_b->priority;

    return pri_diff != 0 ? pri_diff : job_a->arrival_time - job_b->arrival_time;
}

int comparer_plrtf(const void *a, const void *b) {
    job_t *job_a = (job_t *)a;
    job_t *job_b = (job_t *)b;
    job_a->priority = job_a->remain_time;
    job_b->priority = job_b->remain_time;
    int pri_diff = job_a->priority - job_b->priority;

    return pri_diff != 0 ? pri_diff : job_a->arrival_time - job_b->arrival_time;
}

int comparer_rr(const void *a, const void *b) {
    /*job_t *job_a = (job_t *)a;*/
    /*job_t *job_b = (job_t *)b;*/
    /*job_a->priority = job_a->cycle_start_time;*/
    /*job_b->priority = job_b->cycle_start_time;*/
    /*int pri_diff = job_a->priority - job_b->priority;*/

    /*return pri_diff;*/
    return 1;
}

int comparer_sjf(const void *a, const void *b) {
    job_t *job_a = (job_t *)a;
    job_t *job_b = (job_t *)b;
    job_a->priority = job_a->running_time;
    job_b->priority = job_b->running_time;
    int pri_diff = job_a->priority - job_b->priority;

    return pri_diff != 0 ? pri_diff : job_a->arrival_time - job_b->arrival_time;
}

void scheduler_start_up(scheme_t s) {
    switch (s) {
    case FCFS:
        comparison_func = comparer_fcfs;
        break;
    case PRI:
        comparison_func = comparer_pri;
        break;
    case PPRI:
        comparison_func = comparer_ppri;
        break;
    case PLRTF:
        comparison_func = comparer_plrtf;
        break;
    case RR:
        comparison_func = comparer_rr;
        break;
    case SJF:
        comparison_func = comparer_sjf;
        break;
    default:
        printf("Did not recognize scheme\n");
        exit(1);
    }
    priqueue_init(&pqueue, comparison_func);
    scheme = s;
}

bool scheduler_new_job(int job_number, unsigned time, unsigned running_time,
                       int priority) {

    job_t *new_job = (job_t *)malloc(sizeof(job_t));
    new_job->id = job_number;
    new_job->priority = priority;
    new_job->arrival_time = time;
    new_job->running_time = running_time;
    new_job->remain_time = running_time;
    new_job->quantum = 0;
    new_job->cycle_start_time = 0;
    new_job->start_time = time;
    new_job->runned = 0;

    if (core.curr!= NULL) {
        core.curr->remain_time = core.curr->remain_time - (time - core.curr->cycle_start_time);
    }

    if (core.curr == NULL) {
        core.curr = new_job;
        core.curr->start_time = time;
        core.curr->cycle_start_time = time;
        core.curr->runned = 1;
        return true;
    } else if ((scheme == PPRI || scheme == PLRTF) && (comparison_func(new_job, core.curr)) < 0) {
        core.curr->quantum = 0;
        priqueue_offer(&pqueue, core.curr);
        core.curr = new_job;
        core.curr->start_time = time;
        core.curr->cycle_start_time = time;
#ifdef DEBUG
        printf("~~~\n");
        printf("at: %u\n", core.curr->arrival_time);
        printf("st: %u\n", core.curr->start_time);
        printf("~~~\n");
#endif
        core.curr->cycle_start_time = time;
        core.curr->runned = 1;
        return true;
    } else {
        priqueue_offer(&pqueue, new_job);
        return false;
    }

}

int find_next_job (unsigned time) {
    core.curr = (job_t *)priqueue_poll(&pqueue);

    if (core.curr == NULL) {
        return -1;
    }

    if (core.curr->runned == 0) {
        core.curr->start_time = time;
    }

    core.curr->cycle_start_time = time;

    core.curr->runned = 1;
    return core.curr->id;
}

void update_stat(int time) {
    total_waiting_time += (time - core.curr->arrival_time - core.curr->running_time);
    total_response_time += (core.curr->start_time - core.curr->arrival_time);
    total_turnaround_time += (time - core.curr->arrival_time);
}

int scheduler_job_finished(int job_number, unsigned time) {
    if (core.curr != NULL && core.curr->id == job_number) {
        update_stat(time);
    }

    core.curr->quantum = 0;
    num_served++;

    return find_next_job(time);
}

int scheduler_quantum_expired(unsigned time) {
    core.curr->quantum = 0;
    core.curr->remain_time = core.curr->remain_time - (time - core.curr->cycle_start_time);
    priqueue_offer(&pqueue, core.curr);
    return find_next_job(time);
}

float scheduler_average_waiting_time() {
    return (float) total_waiting_time / (float) num_served;
}

float scheduler_average_turnaround_time() {
    return (float) total_turnaround_time / (float) num_served;
}

float scheduler_average_response_time() {
    return (float) total_response_time / (float) num_served;
}

void scheduler_clean_up() {
    priqueue_destroy(&pqueue);
}

void scheduler_show_queue() {
    // This function is left entirely to you! Totally optional.
}
