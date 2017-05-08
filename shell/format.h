/**
 * Machine Problem: Shell
 * CS 241 - Fall 2016
 */
#pragma once
#include <stdio.h>
#include <stdio.h>
#include <sys/types.h>

/**
 * This library will handle all the formatting
 * for your shell.
 *
 * Please use this to ensure your formatting
 * matches what the autograder expects.
 */

#define STATUS_RUNNING "Running"
#define STATUS_STOPPED "Stopped"

/**
 * Print the correct usage on invalid command line arguments.
 */
void print_usage();

/**
 * Print a command and newline.
 */
void print_command(const char *command);

/**
 * Print when the user specifies a non-existent script file.
 */
void print_script_file_error();

/**
 * Print the shell prompt with current directory `directory` and current
 * process id `pid`.
 */
void print_prompt(const char *directory, pid_t pid);

/**
 * Print when the user tries to change directories but `path` does not exist.
 */
void print_no_directory(const char *path);

/**
 * Print when executing an external command with a process id `pid`.
 */
void print_command_executed(pid_t pid);

/**
 * Print when fork fails.
 */
void print_fork_failed();

/**
 * Print when exec fails.
 */
void print_exec_failed(const char *command);

/**
 * Print when wait fails.
 */
void print_wait_failed();

/**
 * Print when use of built-in command is invalid,
 * such as `kill` without pid.
 */
void print_invalid_command(const char *command);

/**
 * Print information about the process.
 *
 * status   STATUS_STOPPED/STATUS_RUNNING
 * pid      process id
 * command  command process is executing
 */
void print_process_info(const char *status, int pid, const char *command);

/**
 * Print when an action on pid is requested, but a process with that
 * pid does not exist, such as `kill 123424235`
 */
void print_no_process_found(int pid);

/**
 * Print when a process was successfully killed.
 *
 * pid      process id of killed process
 * command  command process was running
 */
void print_killed_process(int pid, char *command);

/**
 * Print when process with pid has been stopped.
 */
void print_stopped_process(int pid, char *command);

/**
 * Splits a copy of 'str' by a 'delim'.
 * Then writes the number of tokens into 'numtokens'.
 * These strings are allocated on the heap, so call free_args(char **args) when
 * you are done with them.
 * Learn more at:
 * https://www.quora.com/How-do-you-write-a-C-program-to-split-a-string-by-a-delimiter
 */
char **strsplit(const char *str, const char *delim, size_t *numtokens);

/**
 * Frees the memory returned by strsplit().
 */
void free_args(char **args);

/**
 * Returns the full path a file named 'filename' in the current directory.
 * The path is stored on the heap, so it is up the caller to free it.
 */
char *get_full_path(char *filename);
