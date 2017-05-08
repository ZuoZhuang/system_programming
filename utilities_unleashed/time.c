/**
 * Lab: Utilities Unleashed
 * CS 241 - Spring 2017
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "format.h"
#include <sys/types.h>
#include <sys/wait.h>

#include <time.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_time_usage();
        return 1;
    }

    struct timespec start_time;
    struct timespec end_time;

    if (clock_gettime(CLOCK_MONOTONIC, &start_time) != 0 ) {
        perror("Get start time failed");
        return 1;
    }

    int childid = fork();

    if (childid == -1) {
        print_fork_failed();
        exit(EXIT_FAILURE);
    }
    else if (childid == 0) {
        //TODO
        if (execvp(*(argv + 1), argv + 1) == -1) {
            print_exec_failed();
            exit(EXIT_FAILURE);
        }
        else {
            exit(0);
        }
    }
    else {

        int status;
        waitpid(childid, &status, 0);

        if (!WIFEXITED(status)) {
            return 1;
        }
        else {
            if (clock_gettime(CLOCK_MONOTONIC, &end_time) != 0 ) {
                perror("Get end time failed");
                return 1;
            }
            else {
                double duration = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec) / 1000000000.0;

                display_results(argv, duration);

                return 0;
            }
        }

    }

    return 0;
}
