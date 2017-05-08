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
#include <string.h>
#include <strings.h>
#include <ctype.h>

extern char** environ;

int main(int argc, char *argv[]) {
    if (argc == 1) {
        char **ptr = environ;
        while (*ptr != 0) {
            printf("%s\n", *ptr);
            ptr++;
        }

        return 0;
    }

    if (argc == 2) {
        print_env_usage();
        return 1;
    }
    else {
        int childid = fork();

        if (childid == -1) {
            print_fork_failed();
            exit(EXIT_FAILURE);
        }
        else if (childid == 0) {
            char **set_array = (char **) malloc(sizeof(char *));
            int array_size = 1;
            *set_array = NULL;

            char *str = strtok(argv[1], ",");

            while (str != NULL) {
                set_array = (char **) realloc(set_array, sizeof(char *) * (array_size+1));

                *(set_array + array_size - 1) = str;
                *(set_array + array_size) = NULL;

                array_size++;

                str = strtok(NULL, ",");
            }

            char **ptr = set_array;

            while (*ptr != NULL) {
                char *equal_sign = index(*ptr, '=');

                if (equal_sign == NULL) {
                    print_env_usage();
                    exit(1);
                }
                else {
                    // parse and change env value
                    /*char *destvar = strtok(*ptr, "=");*/
                    /*char *value = strtok(NULL, "=");*/

                    char *destvar;
                    char *value;

                    //TODO replace svars to value
                    /*if (index(*ptr, '%') != NULL) {*/
                    if (1) {
                        char *temp = (char *) malloc(sizeof(char));
                        *temp = '\0';
                        int temp_size = 1;
                        char *svar = (char *) malloc(sizeof(char));
                        *svar = '\0';
                        int svar_size = 1;
                        char *pptr = *ptr;
                        int is_svar = 0;

                        while(*pptr != '\0') {
                            /*printf("------\n");*/
                            /*printf("*pptr: \"%c\"\n", *pptr);*/
                            if(*pptr == '%' && is_svar == 0) {
                                is_svar = 1;
                            }
                            else if (is_svar == 1 && (isalpha(*pptr) || isdigit(*pptr) || *pptr == '_')) {
                                svar = (char *) realloc(svar, sizeof(char)*(svar_size+1));
                                *(svar + svar_size-1) = *pptr;
                                *(svar + svar_size) = '\0';
                                svar_size++;
                                is_svar = 1;
                            }
                            else if (is_svar == 1){
                                /*printf("this is test1\n");*/
                                char *svar_value = getenv(svar);

                                if (svar_value == NULL) {

                                    temp = (char *) realloc(temp, strlen(temp) + 3);
                                    strcat(temp, "");
                                    temp_size = strlen(temp);
                                }
                                else {
                                    temp = (char *) realloc(temp, strlen(temp) + strlen(svar_value) + 1);
                                    strcat(temp, svar_value);
                                    temp_size = strlen(temp);
                                }

                                temp = (char *) realloc(temp, temp_size + 1);
                                if(*pptr == '/') {
                                    /*printf("/ \n");*/
                                    temp[temp_size] = '/';
                                }else
                                temp[temp_size] = *pptr;
                                temp[temp_size+1] = '\0';
                                temp_size+=2;

                                is_svar = 0;
                            }
                            else{
                                /*printf("appending: %c\n", *pptr);*/
                                temp = (char *) realloc(temp, sizeof(char)*(temp_size+1));
                                *(temp + temp_size-1) = *pptr;
                                *(temp + temp_size) = '\0';
                                temp_size++;
                                is_svar = 0;
                            }

                            /*printf("is_svar: %d\n", is_svar);*/
                            pptr++;
                        }

                        if (is_svar == 1) {
                            char *svar_value = getenv(svar);

                            /*if (svar_value == NULL) {*/

                                /*temp = (char *) realloc(temp, strlen(temp) + strlen(svar) + 2);*/
                                /*strcat(temp, "%");*/
                                /*strcat(temp, svar);*/
                                /*temp_size = strlen(temp) + strlen(svar) + 1;*/
                            /*}*/
                            /*else {*/

                                /*temp = (char *) realloc(temp, strlen(temp) + strlen(svar_value) + 1);*/
                                /*strcat(temp, svar_value);*/
                                /*temp_size = strlen(temp) + strlen(svar_value) + 1;*/
                            /*}*/

                            /*printf("this is test\n");*/
                            if (svar_value == NULL) {

                                temp = (char *) realloc(temp, strlen(temp) + strlen(svar) + 2);
                                strcat(temp, "%");
                                strcat(temp, svar);
                                temp_size = strlen(temp);
                            }
                            else {
                                temp = (char *) realloc(temp, strlen(temp) + strlen(svar_value) + 1);
                                strcat(temp, svar_value);
                                temp_size = strlen(temp);
                            }

                            temp = (char *) realloc(temp, temp_size + 1);
                            if(*pptr == '/') {
                                /*printf("/ \n");*/
                                temp[temp_size] = '/';
                            }else
                            temp[temp_size] = *pptr;
                            temp[temp_size+1] = '\0';
                            temp_size++;

                            is_svar = 0;
                        }

                        destvar = strtok(temp, "=");
                        value = strtok(NULL, "=");
                        value = value == NULL ? "" : value;
                    }

                    /*printf("%s, %s\n", destvar, value);*/
                    if (setenv(destvar, value, 1) != 0){
                        print_environment_change_failed();
                        exit(1);
                    }
                }

                ptr++;
            }

            if (execvp(*(argv + 2), argv + 2) == -1) {
                print_exec_failed();
                exit(EXIT_FAILURE);
            }
            else {
                exit(0);
            }


            // TODO Free memory space
            /*set_array = NULL;*/

            exit(0);
        }
        else {
            int status;
            waitpid(childid, &status, 0);

            if (!WIFEXITED(status)) {
                return 1;
            }
            else {
                return 0;
            }
        }
    }

    return 0;
}
