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
#include <sys/stat.h>

#define BUFF_SIZE 1000

#define DEBUG 1

char **parse_args(int argc, char **argv);
verb check_args(char **args);

int check_success(char buff[]) {

    char *response = strtok(buff, "\n");

    if (strlen(response) < 2) {
        return 1;
    } else if (strcmp(response, "OK") == 0) {
        return 0;
    } else if (strlen(response) < 5) {
        return 1;
    } else if (strcmp(response, "ERROR") == 0){
        char *error_mesg = strtok(NULL, "\n");
        print_error_message(error_mesg);
        return -1;
    } else {
        print_invalid_response();
        return -1;
    }
}

int output_read(int sock_fd, FILE *file) {
    size_t num_read = 0;
    int curr_read = 0;
    int first_time = 0;
    size_t first_read = 0;
    size_t num_to_write = 0;

    char buff[BUFF_SIZE];
    size_t size;

    memset(buff, 0, BUFF_SIZE);
    while ((curr_read = read(sock_fd, buff+first_read, sizeof(buff)-first_read)) != 0) {
        /*if(curr_read == -1 && errno == EINTR){*/
            /*continue;*/
        /*}*/

        /*if(curr_read == -1 && errno != EINTR){*/
            /*return -1;*/
        /*}*/

        if(curr_read == -1 && errno == EINTR){
            continue;
        }

        if(curr_read == -1 && errno == EAGAIN){
            continue;
        }

        if(curr_read == -1 && errno != EINTR && errno != EAGAIN){
            return -1;
        }


        num_to_write += curr_read;
        num_read += curr_read;
        char *to_write = buff;

        if (first_time == 0) {
            first_read += curr_read;

            if (first_read < 3+sizeof(size_t)) {
                continue;
            }

            if (check_success(strdup(buff)) != 0) {
                return -1;
            }

            char *rest_read = buff+3;
            memcpy(&size, rest_read, sizeof(size_t));
            printf("should recieve: %lu\n", size);

            /*to_write = buff+3+sizeof(size_t);*/
            to_write = rest_read+sizeof(size_t);

            /*curr_read = curr_read - 3 - sizeof(size_t);*/
            num_to_write = num_to_write - 3 - sizeof(size_t);
            num_read = num_read - 3 - sizeof(size_t);
            first_time++;
        }

        /*printf("to_write: %s\n", to_write);*/

        size_t num_write = 0;
        int curr_write = 0;

        /*while ((curr_write = fwrite(to_write+num_write, 1, curr_read-num_write, file)) != 0) {*/
        while ((curr_write = write(fileno(file), to_write+num_write, num_to_write-num_write)) != 0) {
            /*fflush(file);*/
            if(curr_write == -1 && errno == EINTR){
                continue;
            }

            if(curr_write == -1 && errno != EINTR){
                return -1;
            }


            num_write += curr_write;
        }

        memset(buff, 0, BUFF_SIZE);
        first_read = 0;
        num_to_write = 0;
    }

    printf("actually read: %lu\n", num_read);

    if (size < num_read) {
        print_recieved_too_much_data();
        perror("TOO MUCH: ");
        return -1;
    }

    if (size > num_read) {
        print_too_little_data();
        perror("TOO LITTLE: ");
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
    printf("file_size of %s: %lu\n", filename, st.st_size);

    write(sock_fd, "PUT ", 4);
    write(sock_fd, filename, strlen(filename));
    write(sock_fd, "\n", 1);
    write(sock_fd, (char *)(&size), sizeof(size_t));

    /*char head_buff[BUFF_SIZE];*/
    /*sprintf(head_buff, "PUT %s\n", filename);*/
    /*write(sock_fd, head_buff, strlen(head_buff));*/
    /*write(sock_fd, (char *)(&size), sizeof(size_t));*/

    size_t num_read = 0;
    int curr_read = 0;

    char buff[BUFF_SIZE];
    memset(buff, 0, BUFF_SIZE);
    while ((curr_read = fread(buff, 1, BUFF_SIZE, file)) != 0) {
        if(curr_read == -1 && errno == EINTR){
            continue;
        }

        if(curr_read == -1 && errno != EINTR){
            return -1;
        }

        size_t num_write = 0;
        int curr_write = 0;

        while ((curr_write = write(sock_fd, buff+num_write, curr_read-num_write)) != 0) {
            if(curr_write == -1 && errno == EINTR){
                continue;
            }

            if(curr_write == -1 && errno != EINTR){
                return -1;
            }

            num_write += curr_write;
        }

        num_read += curr_read;
        memset(buff, 0, BUFF_SIZE);
    }

    shutdown(sock_fd, SHUT_WR);

    char response_buff[BUFF_SIZE];
    read(sock_fd, response_buff, sizeof(response_buff));


    if (num_read != size || check_success(response_buff) != 0) {
        printf("Wrong size\n");
        return -1;
    }

    return 0;
}

void free_args(char **args) {
    while (args != NULL) {
        free(args);
        args++;
    }
}

int main(int argc, char **argv) {

    char **args = parse_args(argc, argv);

    if (args == NULL) {
        print_client_usage();
        return 0;
    }

    // Connect Server
    int s;
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct addrinfo hints, *result;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    s = getaddrinfo(args[0], args[1], &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        shutdown(sock_fd, SHUT_RDWR);
        freeaddrinfo(result);
        /*free_args(args);*/
        exit(1);
    }

    if (connect(sock_fd, result->ai_addr, result->ai_addrlen) == -1) {
        perror("Connect Error\n");
        shutdown(sock_fd, SHUT_RDWR);
        freeaddrinfo(result);
        /*free_args(args);*/
        exit(1);
    }

    if (strcmp(args[2], "LIST") == 0) {
        // LIST
        write(sock_fd, "LIST\n", 5);
        shutdown(sock_fd, SHUT_WR);

        if (output_read(sock_fd, stdout) != 0) {
            shutdown(sock_fd, SHUT_RDWR);
            freeaddrinfo(result);
            /*free_args(args);*/
            exit(1);
        }

        print_success();
    } else if (strcmp(args[2], "GET") == 0) {
        // GET
        if (args[3] == NULL || args[4] == NULL) {
            print_client_help();
            shutdown(sock_fd, SHUT_RDWR);
            freeaddrinfo(result);
            /*free_args(args);*/
            exit(1);
        }

        char buff[BUFF_SIZE];
        memset(buff, 0, BUFF_SIZE);
        sprintf(buff, "GET %s\n", args[3]);

        write(sock_fd, buff, strlen(buff));
        shutdown(sock_fd, SHUT_WR);

        FILE *file = fopen(args[4], "w+");

        if (output_read(sock_fd, file) != 0) {
            shutdown(sock_fd, SHUT_RDWR);
            freeaddrinfo(result);
            /*free_args(args);*/
            exit(1);
        }

        print_success();
    } else if (strcmp(args[2], "PUT") == 0) {
        // TODO PUT
        if (args[3] == NULL || args[4] == NULL) {
            print_client_help();
            shutdown(sock_fd, SHUT_RDWR);
            freeaddrinfo(result);
            /*free_args(args);*/
            exit(1);
        }

        FILE *file = fopen(args[4], "r");
        if (file == NULL) {
            printf("File not exist!\n");
            shutdown(sock_fd, SHUT_RDWR);
            freeaddrinfo(result);
            /*free_args(args);*/
            exit(1);
        }

        if (upload_file(sock_fd, args[3], file) != 0) {
            shutdown(sock_fd, SHUT_RDWR);
            freeaddrinfo(result);
            /*free_args(args);*/
            exit(1);
        }

        print_success();
    } else if (strcmp(args[2], "DELETE") == 0) {
        // DELETE

        if (args[3] == NULL) {
            print_client_help();
            freeaddrinfo(result);
            /*free_args(args);*/
            exit(1);
        }

        char buff[BUFF_SIZE];
        memset(buff, 0, BUFF_SIZE);
        sprintf(buff, "DELETE %s\n", args[3]);

        write(sock_fd, buff, strlen(buff));
        shutdown(sock_fd, SHUT_WR);

        char read_buff[BUFF_SIZE];
        read(sock_fd, read_buff, BUFF_SIZE);
        if (check_success(read_buff) != 0){
            shutdown(sock_fd, SHUT_RDWR);
            freeaddrinfo(result);
            /*free_args(args);*/
            exit(1);
        }

        print_success();
    } else {
        // OTHER
        print_client_help();
        shutdown(sock_fd, SHUT_RDWR);
        freeaddrinfo(result);
        /*free_args(args);*/
        exit(1);
    }

    shutdown(sock_fd, SHUT_RDWR);
    freeaddrinfo(result);
    /*free_args(args);*/
    free(args);
    return 0;
}

/**
 * Given commandline argc and argv, parses argv.
 *
 * argc argc from main()
 * argv argv from main()
 *
 * Returns char* array in form of {host, port, method, remote, local, NULL}
 * where `method` is ALL CAPS
 */
char **parse_args(int argc, char **argv) {
  if (argc < 3) {
    return NULL;
  }

  char *host = strtok(argv[1], ":");
  char *port = strtok(NULL, ":");
  if (port == NULL) {
    return NULL;
  }

  char **args = calloc(1, (argc + 1) * sizeof(char *));
  args[0] = host;
  args[1] = port;
  args[2] = argv[2];
  char *temp = args[2];
  while (*temp) {
    *temp = toupper((unsigned char)*temp);
    temp++;
  }
  if (argc > 3) {
    args[3] = argv[3];
  } else {
    args[3] = NULL;
  }
  if (argc > 4) {
    args[4] = argv[4];
  } else {
    args[4] = NULL;
  }

  return args;
}

/**
 * Validates args to program.  If `args` are not valid, help information for the
 * program is printed.
 *
 * args     arguments to parse
 *
 * Returns a verb which corresponds to the request method
 */
verb check_args(char **args) {
  if (args == NULL) {
    print_client_usage();
    exit(1);
  }

  char *command = args[2];

  if (strcmp(command, "LIST") == 0) {
    return LIST;
  }

  if (strcmp(command, "GET") == 0) {
    if (args[3] != NULL && args[4] != NULL) {
      return GET;
    }
    print_client_help();
    exit(1);
  }

  if (strcmp(command, "DELETE") == 0) {
    if (args[3] != NULL) {
      return DELETE;
    }
    print_client_help();
    exit(1);
  }

  if (strcmp(command, "PUT") == 0) {
    if (args[3] == NULL || args[4] == NULL) {
      print_client_help();
      exit(1);
    }
    return PUT;
  }

  // Not a valid Method
  print_client_help();
  exit(1);
}
