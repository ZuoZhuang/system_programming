/**
* Know Your Tools
* CS 241 - Spring 2017
*/
#pragma once
#include <sys/types.h>

/*
* Takes a file and overwrites it multiple times
* With random data.
*/
int shred(const char *filename);

/*
* Allocates a string of length `size` of random
* characters.
*/
char *random_string(ssize_t size);

/*
* Writes `num_bytes` number of characters to the
* file descriptor fd starting from the beginning.
*/
void write_random(int fd, ssize_t num_bytes);