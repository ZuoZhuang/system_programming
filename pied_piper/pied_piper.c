/**
 * CS 241 - Systems Programming
 *
 * Pied Piper - Spring 2017
 */
#include "pied_piper.h"
#include "utils.h"
#include <fcntl.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define TRIES 3

#include <stdio.h>

#define BUFF_SIZE 2000

#define DEBUG 1

int execution(int input_fd, int output_fd, char **executables, int tries) {
    if(output_fd < 0) {
        output_fd = 1;
    }

    int i = 0;

    if (input_fd < 0) {
        input_fd = 0;
    }

    while (executables[i + 1] != NULL) {
        int fd[2];
        pipe(fd);

        int errorfd[2];
        pipe(errorfd);

        pid_t child = fork();

        if (child < 0) {
            return -1;
        }

        if (child > 0) {
            int status;
            waitpid(child, &status, 0);

            if (tries == 2) {
                dup2(errorfd[0], 0);
                char err_info[BUFF_SIZE];
                read(errorfd[0], err_info, BUFF_SIZE);

                failure_information *finfo = NULL;
                finfo->command = executables[i];
                finfo->status = status;
                finfo->error_message = err_info;

                print_failure_report(finfo, tries);

                close(errorfd[0]);
                close(errorfd[1]);
            }

            dup2(fd[0], 0);

            close(fd[0]);
            close(fd[1]);

            if (status != 0) {
                return -1;
            }

        } else {
            dup2(fd[1], 1);
            dup2(errorfd[1], 2);

            if(i == 0 && input_fd != 0) {
                if(dup2(input_fd, 0) != 0) {
                    print_invalid_input();
                    exit(-1);
                }
            }

            if(exec_command(executables[i]) != 0) {
                exit(-1);
            }

            exit(0);
        }

        i++;
    }

    if (output_fd != 1) {
        if(dup2(output_fd, 1) != 1) {
            print_invalid_output();
            return -1;
        }
    }

    return exec_command(executables[i]);
}

int pied_piper(int input_fd, int output_fd, char **executables) {
    // Your code goes here...

    int stdin_backup = dup(0);
    int stdout_backup = dup(1);

    int tries = 0;

    while (execution(input_fd, output_fd, executables, tries) != 0 && tries < TRIES) {
        tries++;
        dup2(stdin_backup, 0);
        dup2(stdout_backup, 1);

        reset_file(input_fd);
        reset_file(output_fd);
    }

    if (tries >= TRIES) {
        return EXIT_OUT_OF_RETRIES;
    }

    return EXIT_SUCCESS;
}
