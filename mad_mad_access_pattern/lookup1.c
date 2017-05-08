/**
 * Mad Mad Access Pattern
 * CS 241 - Spring 2017
 */
#include "tree.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WORD_SIZE 1000

/*#define DEBUG*/

/*
  Look up a few nodes in the tree and print the info they contain.
  This version uses fseek() and fread() to access the data.

  ./lookup1 <data_file> <word> [<word> ...]
*/

uint32_t find_node(uint32_t node, char *target, FILE *file, size_t file_size) {
    if (node > file_size) {
        return 0;
    }

    fseek(file, node, SEEK_SET);
    uint32_t left_node;
    fread(&left_node, 4, 1, file);

    uint32_t right_node;
    fread(&right_node, 4, 1, file);

    uint32_t count;
    fread(&count, 4, 1, file);

    float price;
    fread(&price, 4, 1, file);

    char word[WORD_SIZE];
    fread(word, WORD_SIZE, 1, file);

    int cmp_res = strcmp(word, target);
    if (cmp_res == 0) {
        printFound(word, count, price);
        return node;
    }

    uint32_t left_res = 0;
    uint32_t right_res = 0;

    if (cmp_res < 0) {
        if (right_node != 0) {
            right_res = find_node(left_node, target, file, file_size);
        } else {
            return 0;
        }
    } else if (cmp_res > 0) {
        if (left_node != 0) {
            left_res = find_node(left_node, target, file, file_size);
        } else {
            return 0;
        }
    }


    if (left_res != 0) {
        return left_res;
    }

    if (right_res != 0) {
        return right_res;
    }

    return 0;
}

int main(int argc, char **argv) {
    if (argc < 3) {
        printArgumentUsage();
        exit(1);
    }

    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        openFail(argv[1]);
        exit(1);
    }
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char head[5];
    size_t num_read = fread(head, 1, 4, file);
    head[4] = '\0';
    if (num_read != 4 || (strcmp(head, "BTRE") != 0)) {
        formatFail(argv[1]);
        exit(1);
    }

    for (int i = 2; i < argc; i++) {
        uint32_t target_node = find_node(4, argv[i], file, file_size);

        if (target_node == 0) {
            printNotFound(argv[i]);
        }

        fseek(file, 0, SEEK_SET);
    }

    fclose(file);
}
