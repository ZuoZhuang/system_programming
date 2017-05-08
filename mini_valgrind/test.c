/**
 * Mini Valgrind Lab
 * CS 241 - Spring 2017
 */

#include <stdio.h>
#include <stdlib.h>

int main() {
  // your tests here using malloc and free

    void *ptr1 = malloc(50);
    void *ptr2 = calloc(20, 10);
    void *ptr3 = realloc(ptr1, 10);
    free(ptr1);
    free(ptr1);
  return 0;
}
