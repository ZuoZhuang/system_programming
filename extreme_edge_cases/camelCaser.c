/**
 * Extreme Edge Cases Lab
 * CS 241 - Spring 2017
 */
#include "camelCaser.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

char **camel_caser(const char *input_str) {
    char *b = (char *) malloc(strlen(input_str) + 1);
    char **output_s = (char**) malloc(sizeof(char *));
    *output_s = b;
    int arr_len = 1;

    int n_char = 0;
    int n_word = 0;

    while (*input_str != '\0') {
        if(isalpha(*input_str) && n_word == 0 && n_char == 0) {
            *b++ = tolower(*input_str);
            n_word++;
            n_char++;
        }
        else if(isalpha(*input_str) && n_word != 0 && n_char == 0) {
            *b++ = toupper(*input_str);
            n_char++;
        }
        else if (isdigit(*input_str)) {
            *b++ = *input_str;
            if (*(input_str+1) == ' ') {
                n_word++;
            }
        }
        else if (isalpha(*input_str)) {
            *b++ = tolower(*input_str);
            n_char++;
        }
        else if (isspace(*input_str)) {
            if (n_word != 0)
                n_word++;

            n_char = 0;
        }
        else if (ispunct(*input_str)) {
            *b++ = '\0';

            if (*(input_str+1) != '\0') {
                output_s = (char **) realloc(output_s, sizeof(char *) * ++arr_len);
                *(output_s+arr_len-1) = b;
            }
            else
                output_s = (char **) realloc(output_s, sizeof(char *) * ++arr_len);

            n_word = 0;
            n_char = 0;
        }
        else {
            *b++ = *input_str;
            n_char++;
        }

        input_str++;
    }

    *(output_s+arr_len-1) = NULL;
    output_s = (char **) realloc(output_s, sizeof(char *) * ++arr_len);
    *(output_s+arr_len-1) = NULL;

    return output_s;
}
