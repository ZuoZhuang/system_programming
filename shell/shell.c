/**
 * Machine Problem: Shell
 * CS 241 - Fall 2016
 */
#include "format.h"
#include "shell.h"
#include "vector.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct process {
  char *command;
  char *status;
  pid_t pid;
} process;

void sigint_handler(int sig){
    // TODO check foreground process and kill it
    return;
}

void cleanup(){
    // TODO cleanup
    return;
}

int shell(int argc, char *argv[]) {
    FILE *fd;
    char cwd[1024];
    int from_file = 0;

    signal(SIGINT, sigint_handler);
    if (argc > 3 || argc == 2) {
        print_usage();
        return 0;
    }

    if (argc ==3 && strcmp(argv[1], "-f") == 0) {
        fd = fopen(argv[2], "r");

        if (fd == NULL) {
            print_script_file_error();
            return 0;
        }

        from_file = 1;
    }
    else {
        fd = stdin;
        from_file = 0;
    }

    while(1) {
        fflush(stdout);
        getcwd(cwd, sizeof(cwd));
        print_prompt(cwd, getpid());
        fflush(stdout);

        char *arg_str = NULL;
        size_t num = 0;

        ssize_t num_read = getline(&arg_str, &num, fd);

        if (num_read == -1){
            cleanup();
            fclose(fd);
            free(arg_str);
            /*kill(0, SIGKILL);*/
            return 0;
        }


        if (num_read > 0 && arg_str[num_read -1] == '\n' ) {
            arg_str[num_read-1] = '\0';
            num_read--;
        }

        /*printf("num_read: %d\n", (int) num_read);*/

        if (num_read > 0) {
            int bg_flag = 0;

            if (from_file == 1) {
                print_command(arg_str);
            }

            if (arg_str[num_read-1] == '&') {
                bg_flag = 1;
                arg_str[num_read-1] = '\0';
                num_read--;
            }

            size_t argc_z;
            char **argv_z = strsplit(arg_str, " ", &argc_z);

            if (strcmp(*argv_z, "cd") == 0) {
                if (chdir(*(argv_z+1)) == -1){
                    print_no_directory(*(argv_z+1));
                    continue;
                }
                continue;
            }

            if (strcmp(*argv_z, "ps") == 0) {
                // TODO show process status
                const char *status = "Running";
                print_process_info(status, 1, arg_str);

                /*execvp("ps", ");*/
                continue;
            }

            if (strcmp(*argv_z, "kill") == 0) {
                pid_t target_pid = (pid_t) atoi(*(argv_z+1));

                // TODO 1. Check non existed pid
                /*print_no_process_found(target_pid);*/

                // TODO 2. Check if there is no pid at all
                /*print_invalid_command(arg_str);*/

                if (kill(target_pid, SIGTERM) == -1) {
                    print_no_process_found(target_pid);
                }
                // TODO second arg should be in the struct
                print_killed_process(target_pid, arg_str);

                continue;
            }

            if (strcmp(*argv_z, "stop") == 0) {
                pid_t target_pid = (pid_t) atoi(*(argv_z+1));

                // TODO 1. Check non existed pid
                /*print_no_process_found(target_pid);*/

                // TODO 2. Check if there is no pid at all
                /*print_invalid_command(arg_str);*/

                // TODO second arg should be in the struct
                if (kill(target_pid, SIGSTOP) == -1){
                    print_no_process_found(target_pid);
                }
                print_stopped_process(target_pid, arg_str);

                continue;
            }

            if (strcmp(*argv_z, "cont") == 0) {
                pid_t target_pid = (pid_t) atoi(*(argv_z+1));

                // TODO 1. Check non existed pid
                /*print_no_process_found(target_pid);*/

                // TODO 2. Check if there is no pid at all
                /*print_invalid_command(arg_str);*/

                // TODO second arg should be in the struct
                if (kill(target_pid, SIGCONT) == -1) {
                    print_no_process_found(target_pid);
                }

                continue;
            }

            if (strcmp(*argv_z, "exit") == 0) {
                cleanup();
                free_args(argv_z);
                fclose(fd);
                free(arg_str);
                /*kill(0, SIGKILL);*/
                return 0;
            }

            fflush(stdout);

            pid_t child = fork();

            if (child < 0) {
                print_fork_failed();
                free_args(argv_z);
                fclose(fd);
                free(arg_str);
                /*kill(0, SIGKILL);*/
                return 0;
            }

            if (child  == 0) {
                // TODO record process in the vector
                if (execvp(*argv_z, argv_z) == -1){
                    print_exec_failed(arg_str);
                }

                exit(0);
            }
            else {
                print_command_executed(child);
                int status;

                if (bg_flag == 0) {
                    if (waitpid(child, &status, 0) == -1) {
                        print_wait_failed();
                    }
                }

                continue;
            }

            free_args(argv_z);
        }


    }

  fclose(fd);
  /*kill(0, SIGKILL);*/
  return 0;
}
