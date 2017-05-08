/**
 * MapReduce
 * CS 241 - Spring 2017
 */

#include "core/utils.h"
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define BUFF_SIZE 1000

/*#define DEBUG 1*/

int main(int argc, char **argv) {

    if (argc < 6) {
        print_usage();
        return 0;
    }

    char *input_file = argv[1];
    int in_file_fd = open(argv[1], O_RDONLY);
    int out_file_fd = open(argv[2], O_WRONLY | O_CREAT);
    char *mapper = argv[3];
    char *reducer = argv[4];
    int n_mappers = atoi(argv[5]);

    int reducer_fds[2];

    pipe(reducer_fds);
    pid_t child = fork();

    if (child < 0) {
        printf("fork failed!\n");
        return -1;
    }

    if (child > 0) {
        // Reducer Process
        close(reducer_fds[1]);
        dup2(reducer_fds[0], 0);

        if (out_file_fd != 1) {
            dup2(out_file_fd, 1);
        }

        // reducer behavior
        if (execlp(reducer, reducer, NULL) == -1) {
            perror("mapper execfailed\n");
        }

        close(reducer_fds[0]);

    } else {
        // Parent process for mapper processes

        // map pipe for reducer
        close(reducer_fds[0]);
        dup2(reducer_fds[1], 1);

        // array to store mapper pids
        pid_t mapper_pids[n_mappers];

        // fork mapper processes
        for (int i = 0; i < n_mappers; i++) {
            pid_t mapper_child = fork();

            if (mapper_child < 0) {
                printf("mapper fork failed!\n");
                exit(-1);
            }

            if (mapper_child > 0) {
                // Parent process for mapper processes

                mapper_pids[i] = mapper_child;
            } else {
                // mapper processes

                pid_t splitter_fds[2];
                pipe(splitter_fds);

                // Fork splitter process
                pid_t splitter_pid = fork();

                if (splitter_pid < 0) {
                    printf("splitter fork failed!\n");
                    exit(-1);
                }

                if (splitter_pid > 0) {
                    // mapper process
                    close(splitter_fds[1]);
                    dup2(splitter_fds[0], 0);
                    dup2(reducer_fds[1] ,1);
                    /*dup2(2, 1);*/

                    // mapper behaviour

                    if (execlp(mapper, mapper, NULL) == -1) {
                        perror("mapper execfailed\n");
                    }

                    close(splitter_fds[0]);
                } else {
                    // splitter process
                    close(splitter_fds[0]);
                    dup2(splitter_fds[1], 1);

                    // splitter behavior
                    char char_mappers[10];
                    char char_idx[10];
                    sprintf(char_mappers, "%d", n_mappers);
                    sprintf(char_idx, "%d", i);

                    if (execlp("./splitter", "./splitter", input_file, char_mappers, char_idx, NULL) == -1) {
                        printf("shit\n");
                    }

                    exit(0);
                }

                exit(0);
            }
        }

        exit(0);
    }

    // Clean up
    close(in_file_fd);
    close(out_file_fd);

    return 0;
}
