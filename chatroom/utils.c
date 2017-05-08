/**
 * Chatroom Lab
 * CS 241 - Spring 2017
 */
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <errno.h>

#include "utils.h"
static const size_t MESSAGE_SIZE_DIGITS = 4;

char *create_message(char *name, char *message) {
    int name_len = strlen(name);
    int msg_len = strlen(message);
    char *msg = calloc(1, msg_len + name_len + 4);
    sprintf(msg, "%s: %s", name, message);

    return msg;
}

ssize_t get_message_size(int socket) {
    int32_t size;
    ssize_t read_bytes =
        read_all_from_socket(socket, (char *)&size, MESSAGE_SIZE_DIGITS);
    if (read_bytes == 0 || read_bytes == -1)
        return read_bytes;

    return (ssize_t)ntohl(size);
}

// You may assume size won't be larger than a 4 byte integer
ssize_t write_message_size(size_t size, int socket) {
    // Your code here

    int32_t size2write = htonl(size);
    ssize_t num_wrote = write(socket, &size2write, sizeof(size2write));

    return num_wrote;
}

ssize_t read_all_from_socket(int socket, char *buffer, size_t count) {
    // Your Code Here

    ssize_t num_read = 0;
    while ((long)num_read < (long)count) {
        ssize_t curr_num_read = read(socket, buffer+num_read, count-num_read);
        if (curr_num_read == -1) {
            if (errno == EINTR) {
                continue;
            }

            return -1;
        }

        if (curr_num_read == 0) {
            break;
        }

        num_read += curr_num_read;
    }
    return num_read;
}

ssize_t write_all_to_socket(int socket, const char *buffer, size_t count) {
    // Your Code Here

    ssize_t num_write = 0;
    while ((long)num_write < (long)count) {
        ssize_t curr_num_write = write(socket, buffer+num_write, count-num_write);
        if (curr_num_write == -1) {
            if (errno == EINTR) {
                continue;
            }

            return -1;
        }

        if (curr_num_write == 0) {
            break;
        }

        num_write += curr_num_write;
    }
    return num_write;
}
