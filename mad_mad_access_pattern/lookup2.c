/**
 * Mad Mad Access Pattern
 * CS 241 - Spring 2017
 */
#include "tree.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define WORD_SIZE 1000

/*#define DEBUG*/

/*
  Look up a few nodes in the tree and print the info they contain.
  This version uses fseek() and fread() to access the data.

  ./lookup1 <data_file> <word> [<word> ...]
*/

BinaryTreeNode *find_node(BinaryTreeNode *node, char *target, void *addr) {
    if (node == NULL) {
        return NULL;
    }

    int cmp_res = strcmp(node->word, target);
    if (cmp_res == 0) {
        return node;
    }

    BinaryTreeNode *left_res = NULL;
    BinaryTreeNode  *right_res = NULL;

    if (cmp_res < 0) {
        if (node->right_child != 0) {
            right_res = find_node((BinaryTreeNode *)(addr+node->left_child), target, addr);
        } else {
            return NULL;
        }
    } else if (cmp_res > 0) {
        if (node->left_child != 0) {
            left_res = find_node((BinaryTreeNode *)(addr+node->left_child), target, addr);
        } else {
            return NULL;
        }
    }

    if (left_res != NULL) {
        return left_res;
    }

    if (right_res != NULL) {
        return right_res;
    }

    return NULL;
}

int main(int argc, char **argv) {
    if (argc < 3) {
        printArgumentUsage();
        exit(1);
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        openFail(argv[1]);
        exit(1);
    }

    /*off_t file_size = lseek(fd, 0, SEEK_END);*/
    /*lseek(fd, 0, SEEK_SET);*/

    struct stat file_stat;
    if (fstat(fd, &file_stat) == -1) {
        perror("fail to get file status: ");
        exit(1);
    }

    void *addr = mmap(0, file_stat.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (addr == (void *)-1) {
        mmapFail(argv[1]);
        exit(1);
    }

    char head[5];
    memcpy(head, addr, 4);
    head[4] = '\0';
    if ((strcmp(head, "BTRE") != 0)) {
        formatFail(argv[1]);
        exit(1);
    }

    for (int i = 2; i < argc; i++) {
        BinaryTreeNode *root = (BinaryTreeNode *)(addr+4);
        BinaryTreeNode *target_node = find_node(root, argv[i], addr);

        if (target_node != NULL) {
            printFound(target_node->word, target_node->count, target_node->price);
        } else {
            printNotFound(argv[i]);
        }
    }

    munmap(addr, file_stat.st_size);
    close(fd);

}
