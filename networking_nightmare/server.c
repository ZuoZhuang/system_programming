/**
 * Networking
 * CS 241 - Spring 2017
 */
#include "common.h"
#include "format.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <ctype.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <signal.h>
#include "includes/vector.h"
#include <sys/stat.h>

#define BUFF_SIZE 1024
#define MAX_CONN 1024
#define MAX_EVENTS 1024

#define PUT 1
#define GET 2
#define DELETE 3
#define LIST 4
#define UNDEF 5

/*#define DEBUG 1*/

int sock_fd;
struct epoll_event *events;
char *dir;
char *temp_dir;
vector *fs_vec;

char *get_filename(char buff[]) {
    char *response = strtok(buff, "\n");
    char *command = strtok(response, " ");
    char *filename = strtok(NULL, " ");
    command = NULL;

    return filename;
}

int output_read(int sock_fd, FILE *file) {
    size_t num_read = 0;
    size_t first_read = 0;
    int curr_read = 0;
    int first_time = 0;
    int num_to_write = 0;

    char read_buff[BUFF_SIZE];
    size_t size;

    memset(read_buff, 0, BUFF_SIZE);
    while ((curr_read = read(sock_fd, read_buff+first_read, sizeof(read_buff)-first_read)) != 0) {
        if(curr_read == -1 && errno == EINTR){
            continue;
        }

        if(curr_read == -1 && errno == EAGAIN){
            continue;
        }

        if(curr_read == -1 && errno != EINTR && errno != EAGAIN){
            return -1;
        }

        num_read += curr_read;
        num_to_write += curr_read;

        char *to_write = read_buff;

        if (first_time == 0) {
            first_read += curr_read;
            if (first_read < sizeof(size_t)) {
                continue;
            }
            char *rest_read = read_buff;
            memcpy(&size, rest_read, sizeof(size_t));

#ifdef DEBUG
            printf("[output_read] size: %lu\n", size);
            printf("------\n");
#endif

            to_write = read_buff+sizeof(size_t);

            /*curr_read = curr_read - sizeof(size_t);*/
            num_to_write = num_to_write - sizeof(size_t);
            num_read = num_read - sizeof(size_t);
            first_time++;
        }

        size_t num_write = 0;
        int curr_write = 0;
        /*while ((curr_write = fwrite(to_write+num_write, 1, curr_read-num_write, file)) != 0) {*/
        while ((curr_write = fwrite(to_write+num_write, 1, num_to_write-num_write, file)) != 0) {
            if(curr_write == -1 && errno == EINTR){
                continue;
            }

            if(curr_write == -1 && errno != EINTR){
                return -1;
            }

            fflush(file);
            num_write += curr_write;
        }


        memset(read_buff, 0, BUFF_SIZE);
        first_read = 0;
        num_to_write = 0;
    }

    memset(read_buff, 0, BUFF_SIZE);
    if (size < num_read) {
        print_recieved_too_much_data();
        write(sock_fd, "ERROR\n", 6);
        write(sock_fd, err_bad_file_size, strlen(err_bad_file_size));
        write(sock_fd, "\n", 1);
        return -1;
    }

    if (size > num_read) {
        print_too_little_data();
        write(sock_fd, "ERROR\n", 6);
        write(sock_fd, err_bad_file_size, strlen(err_bad_file_size));
        write(sock_fd, "\n", 1);
        return -1;
    }

    return 0;
}

int upload_file (int sock_fd, char *filename, FILE *file) {
    /*fseek(file, 0, SEEK_END);*/
    /*size_t size = ftell(file);*/
    /*fseek(file, 0, SEEK_SET);*/

    size_t size;
    struct stat st;
    fstat(fileno(file), &st);
    size = st.st_size;

    write(sock_fd, "OK\n", 3);
    /*write(sock_fd, filename, strlen(filename));*/
    /*write(sock_fd, "\n", 1);*/
    write(sock_fd, (char *)(&size), sizeof(size_t));
    printf("[upload] should send: %lu\n", size);

    size_t num_read = 0;
    int curr_read = 0;
    size_t total_write = 0;

    char buff[BUFF_SIZE];
    memset(buff, 0, BUFF_SIZE);
    while ((curr_read = fread(buff, 1, BUFF_SIZE, file)) != 0) {
        if(curr_read == -1 && errno == EINTR){
            continue;
        }

        if(curr_read == -1 && errno != EINTR){
            return -1;
        }

        num_read += curr_read;

        size_t num_write = 0;
        int curr_write = 0;

        while ((curr_write = write(sock_fd, buff+num_write, curr_read-num_write)) != 0) {
            if(curr_write == -1 && errno == EINTR){
                continue;
            }

            /*if(curr_write == -1 && errno != EINTR){*/
                /*return -1;*/
            /*}*/

            if(curr_write == -1 && errno == EAGAIN){
                continue;
            }

            if(curr_write == -1 && errno != EINTR && errno != EAGAIN){
                return -1;
            }

            num_write += curr_write;
        }


        if ((int)num_write != curr_read) {
            perror("[upload_file] read/write not match: ");
        }

        total_write += num_write;

        memset(buff, 0, BUFF_SIZE);
    }

    printf("[upload] actually send: %lu\n", total_write);

    return 0;
}

int unblock_socket(int fd) {
    int flags, s;

    flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl: ");
        return -1;
    }

    flags |= O_NONBLOCK;
    s = fcntl(fd, F_SETFL, flags);
    if (s == -1) {
        perror("fcntl:");
        return -1;
    }

    return 0;
}

int check_command(char buff[]) {
    char *pos_newline = strchr(buff, '\n');
    if (pos_newline == NULL) {
        return -1;
    }

    char *response = strtok(buff, "\n");
    char *command = strtok(response, " ");

    if (strcmp(command, "PUT") == 0) {
        return PUT;
    } else if (strcmp(command, "GET") == 0) {
        return GET;
    } else if (strcmp(command, "DELETE") == 0) {
        return DELETE;
    } else if (strcmp(command, "LIST") == 0) {
        return LIST;
    } else {
        return UNDEF;
    }
}

void rm_temp_fs(char *dir) {
    size_t num_files = vector_size(fs_vec);
    for (size_t j = 0; j < num_files; j++) {
        char *filename = *(vector_at(fs_vec, j));
        char *path = strcat(strdup(temp_dir), filename);
        unlink(path);
    }

    rmdir(dir);
}

void gracefully_exit(int temp) {
    free(events);
    close(sock_fd);
    rm_temp_fs(dir);
    vector_destroy(fs_vec);
    exit(0);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage : ./server <port>\n");
        return 1;
    }

    signal(SIGINT, gracefully_exit);

    char template[] = "./tempXXXXXX";
    dir = mkdtemp(template);
    if (dir == NULL) {
        perror("make temp failed: ");
    }
    print_temp_directory(dir);
    temp_dir = strcat(strdup(dir), "/");

    fs_vec = vector_create(NULL, NULL, NULL);

    int s;
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    s = getaddrinfo(NULL, argv[1], &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(1);
    }

    int optval = 1;
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &optval, sizeof(optval));

    // bind
    if (bind(sock_fd, result->ai_addr, result->ai_addrlen) != 0) {
        perror("bind error\n");
        exit(1);
    }

    freeaddrinfo(result);

    s = unblock_socket(sock_fd);
    if (s == -1) {
        perror("unblock fd: ");
        exit(1);
    }

    s = listen(sock_fd, MAX_CONN);
    if (s == -1) {
        perror("listen: ");
        exit(1);
    }

    int efd = epoll_create(1);
    if (efd == -1) {
        perror("epoll create: ");
        exit(1);
    }

    struct epoll_event event;
    /*struct epoll_event *events;*/

    event.data.fd = sock_fd;
    event.events = EPOLLIN | EPOLLET;

    if (epoll_ctl (efd, EPOLL_CTL_ADD, sock_fd, &event) == -1) {
        perror("epoll_ctl: ");
        exit(1);
    }

    events = calloc(MAX_EVENTS, sizeof(event));

    // main code for events
    while (1) {
        printf("\nWaiting...\n");

        int num_events = epoll_wait(efd, events, MAX_EVENTS, -1);

        for (int i = 0; i < num_events; i++) {
            if ((events[i].events & EPOLLERR)
                 || (events[i].events & EPOLLHUP)
                 || (!(events[i].events & EPOLLIN))) {
                perror("epoll error: \n");
                close(events[i].data.fd);

                continue;
            } else if (events[i].data.fd == sock_fd) {
                while (1) {
                    struct sockaddr in_addr;
                    socklen_t in_len = sizeof(in_addr);

                    int client_fd = accept(sock_fd, &in_addr, &in_len);
                    if (client_fd == -1) {
                        if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                            break;
                        } else {
                            perror ("accept error: ");
                            break;
                        }
                    }

                    s = unblock_socket(client_fd);
                    if (s == -1) {
                        perror("unblock client fd: ");
                        exit(1);
                    }

                    event.data.fd = client_fd;
                    event.events = EPOLLIN | EPOLLET;
                    if (epoll_ctl(efd, EPOLL_CTL_ADD, client_fd, &event) == -1) {
                        perror("epoll_ctl: ");
                        exit(1);
                    }
                }
                continue;
            } else {

                /*int first_time = 0;*/
                int curr_read = 0;
                int num_read = 0;
                int contain_command = 0;
                /*int command = UNDEF;*/

                char buff[BUFF_SIZE];
                memset(buff, 0, sizeof(buff));
                while ((curr_read = read(events[i].data.fd, buff+num_read, BUFF_SIZE-num_read >= 1 ? 1 : 0)) != 0) {
                    if(curr_read == -1 && errno == EINTR){
                        continue;
                    }

                    if(curr_read == -1 && errno == EAGAIN){
                        continue;
                    }

                    if(curr_read == -1 && errno != EINTR && errno != EAGAIN){
                        break;
                    }
                    /*if(curr_read == -1 && errno != EINTR){*/
                        /*break;*/
                    /*}*/

                    num_read += curr_read;

                    if (buff[num_read-1] == '\n') {
                        contain_command = 1;
                        break;
                    } else {
                        continue;
                    }
                }

                if (contain_command == 1) {
                    int command = check_command(strdup(buff));
                    printf("[command] %d\n", command);

                    buff[num_read-1] = '\0';
                    if (command == -1) {
                        printf("waiting for newline\n");
                        continue;
                    } else if (command == PUT) {
                        // TODO what if there is no filename?
                        char *filename = buff+4;
                        char *path = strcat(strdup(temp_dir), filename);

                        printf("[PUT command] filename: %s\n", path);

                        FILE *file = fopen(path, "w+");
                        if (file == NULL) {
                            perror("file open failed: ");
                        } else {
                            int success = 1;
                            if (output_read(events[i].data.fd, file) != 0) {
                                perror("write to file failed: ");
                                unlink(path);
                                success = 0;
                            }

                            fclose(file);

                            if (success == 1) {
                                size_t num_files = vector_size(fs_vec);
                                int exist_file = 0;
                                size_t j = 0;
                                for (j = 0; j < num_files; j++) {
                                    char *exist_name = *(vector_at(fs_vec, j));
                                    if (strcmp(filename, exist_name) == 0) {
                                        exist_file = 1;
                                        break;
                                    }
                                }

                                if (exist_file == 0) {
                                    vector_push_back(fs_vec, strdup(filename));
                                }
                                write(events[i].data.fd, "OK\n", 3);
                            }
                        }

                    } else if (command == GET) {
                        // TODO what if there is no filename?
                        char *target_name = buff+4;
                        char *file_name = strcat(strdup(temp_dir), target_name);

                        printf("[GET] target_name: %s\n", file_name);

                        FILE *file = fopen(file_name, "r");
                        if (file == NULL) {
                            printf("File not exist!\n");
                            /*shutdown(sock_fd, SHUT_RDWR);*/
                            /*freeaddrinfo(result);*/
                            /*free_args(args);*/
                            /*exit(1);*/
                            write(events[i].data.fd, "ERROR\n", 6);
                            write(events[i].data.fd, err_no_such_file, strlen(err_no_such_file));
                            write(events[i].data.fd, "\n", 1);
                        } else {

                            if (upload_file(events[i].data.fd, target_name, file) != 0) {
                                /*shutdown(sock_fd, SHUT_RDWR);*/
                                /*freeaddrinfo(result);*/
                                /*free_args(args);*/
                                /*exit(1);*/
                                perror("upload failed: ");
                            }

                            fclose(file);
                        }
                    } else if (command == DELETE) {
                        // TODO what if there is no filename?
                        char *target_name = buff+7;

                        printf("[DELETE] target_name: %s\n", target_name);

                        size_t num_files = vector_size(fs_vec);

                        int exist_file = 0;
                        size_t j = 0;
                        for (j = 0; j < num_files; j++) {
                            char *filename = *(vector_at(fs_vec, j));
                            if (strcmp(filename, target_name) == 0) {
                                unlink(strcat(strdup(temp_dir), filename));
                                vector_erase(fs_vec, j);
                                exist_file = 1;
                                break;
                            }
                        }

                        if (exist_file == 1) {
                            write(events[i].data.fd, "OK\n", 3);
                        } else {
                            write(events[i].data.fd, "ERROR\n", 6);
                            write(events[i].data.fd, err_no_such_file, strlen(err_no_such_file));
                            write(events[i].data.fd, "\n", 1);
                        }
                    } else if (command == LIST) {
                        size_t num_files = vector_size(fs_vec);
                        size_t num_to_write = 0;
                        for (size_t j = 0; j < num_files; j++) {
                            num_to_write += strlen(*(vector_at(fs_vec, j)));
                            num_to_write += 1;
                        }

                        if (num_to_write > 0) {
                            num_to_write -= 1;
                        }

                        write(events[i].data.fd, "OK\n", 3);
                        write(events[i].data.fd, (char *)(&num_to_write), sizeof(size_t));

                        size_t j = 0;
                        for (j = 0; j < num_files; j++) {
                            char *filename = *(vector_at(fs_vec, j));

                            size_t num_write = 0;
                            int curr_write = 0;
                            while ((curr_write =write(events[i].data.fd, filename+num_write, strlen(filename)-num_write)) != 0) {
                                if(curr_write == -1 && errno == EINTR){
                                    continue;
                                }

                                if(curr_write == -1 && errno == EAGAIN){
                                    continue;
                                }

                                if(curr_write == -1 && errno != EINTR && errno != EAGAIN){
                                    /*return -1;*/
                                    break;
                                }

                                num_write += curr_write;
                            }

                            if (j < num_files-1) {
                                write(events[i].data.fd, "\n", 1);
                            }
                        }
                    } else {
                        printf("Undefined Command\n");
                        write(events[i].data.fd, "ERROR\n", 6);
                        write(events[i].data.fd, err_bad_request, strlen(err_bad_request));
                        write(events[i].data.fd, "\n", 1);
                        printf("send\n");
                    }
                } else {
                    printf("wrong command format!\n");
                    char err_buff[BUFF_SIZE] = "ERROR\n";
                    strcat(err_buff, err_bad_request);
                    int error_write = write(events[i].data.fd, err_buff, strlen(err_buff));
                    if (error_write == -1) {

                    }

                    /*int error_write = write(events[i].data.fd, "ERROR\n", 6);*/
                    /*if (error_write == -1) {*/

                    /*}*/
                    /*error_write = write(events[i].data.fd, err_bad_request, strlen(err_bad_request));*/
                    /*write(events[i].data.fd, "\n", 1);*/
                    printf("message sent\n");
                }


                shutdown(events[i].data.fd, SHUT_RDWR);
                close(events[i].data.fd);
                print_connection_closed();

                memset(buff, 0, BUFF_SIZE);
                printf("=================\n");
                continue;
            }
        }
    }
    free(events);
    close(sock_fd);
    /*rmdir(dir);*/
    rm_temp_fs(dir);
    gracefully_exit(0);
}
