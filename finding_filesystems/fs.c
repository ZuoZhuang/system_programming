/**
* Finding Filesystems
* CS 241 - Spring 2017
*/
#include "format.h"
#include "fs.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <math.h>

#define READ_OPERATION 0
#define WRITE_OPERATION 1
#define EXE_OPERATION 2

/*#define DEBUG 1*/

// check whether current user has the auth to complete
int check_auth(inode *node, int opertion) {
    // TODO NO NEED, Just Ignore it
    return 1;
}

void ls_helper (file_system *fs,
                inode *node,
                uint64_t node_size,
                uint64_t read_size) {
    dirent *block_info = (dirent *)malloc(sizeof(dirent));
    /*uint64_t read_size = 0;*/
    for (int i = 0; i < NUM_DIRECT_INODES; i++) {
        data_block *curr_block = fs->data_root + node->direct[i];

        char *data_str = curr_block->data;
        while (read_size < node_size) {
            if (make_dirent_from_string(data_str, block_info) == 0) {
                i = NUM_DIRECT_INODES;
                break;
            }

            inode *curr_inode = fs->inode_root + block_info->inode_num;
            if (is_directory(curr_inode)) {
                print_directory(block_info->name);
            } else if (is_file(curr_inode)) {
                print_file(block_info->name);
            } else {
                print_file(block_info->name);
            }

            data_str += 256;
            read_size += 256;
        }
    }

    free(block_info);

    if (read_size < node_size) {
        ls_helper(fs, fs->inode_root + node->indirect, node_size, read_size);
    }
}

void fs_ls(file_system *fs, char *path) {
    // Arrrrrgh Matey
    inode *node = get_inode(fs, path);
    if (node == NULL) {
        print_no_file_or_directory();
        return;
    }

    if (!is_directory(node)) {
        printf("Target is not a directory\n");
        return;
    }

    if (!check_auth(node, EXE_OPERATION)) {
        printf("Permission Denied\n");
        return;
    }

    ls_helper(fs, node, node->size, 0);
}

void cat_helper(file_system *fs,
                inode *node,
                uint64_t node_size,
                uint64_t read_size) {

    dirent *block_info = (dirent *)malloc(sizeof(dirent));

    if (is_directory(node)) {
        for (int i = 0; i < NUM_DIRECT_INODES; i++) {
            data_block *curr_block = fs->data_root + node->direct[i];

            char *data_str = curr_block->data;
            while (read_size < node_size) {
                if (make_dirent_from_string(data_str, block_info) == 0) {
                    i = NUM_DIRECT_INODES;
                    break;
                }

                printf("%s%08d", block_info->name, (int)block_info->inode_num);

                data_str += 256;
                read_size += 256;
            }
        }
    } else {
        for (int i = 0; i < NUM_DIRECT_INODES; i++) {
            data_block *curr_block = fs->data_root + node->direct[i];

            uint64_t num_write = 0;
            if (node_size >= read_size + 16 * KILOBYTE) {
                num_write = write(1, curr_block, 16 * KILOBYTE);
            } else {
                num_write = write(1, curr_block, node_size - read_size);
;
            }

            read_size += num_write;

            /*printf("%s", curr_block->data);*/
            /*read_size += strlen(curr_block->data);*/

            if (read_size >= node_size) {
                break;
            }
        }
    }
    free(block_info);

    if (read_size < node_size) {
        /*perror("redirect\n");*/
        cat_helper(fs, fs->inode_root + node->indirect, node_size, read_size);
    }
}

void fs_cat(file_system *fs, char *path) {
    // Shiver me inodes

    inode *node = get_inode(fs, path);
    if (node == NULL) {
        print_no_file_or_directory();
        return;
    }

    /*if (!is_file(node)) {*/
        /*printf("Target is not a file\n");*/
        /*return;*/
    /*}*/

    if (!check_auth(node, EXE_OPERATION)) {
        printf("Permission Denied\n");
        return;
    }

    cat_helper(fs, node, node->size, 0);
}
