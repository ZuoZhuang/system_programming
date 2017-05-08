/**
 * Parallel Make
 * CS 241 - Spring 2017
 */

#include "format.h"
#include <stdio.h>

void print_cycle_failure(char *target) {
  printf("make: dropped build target '%s' due to circular dependencies\n",
         target);
}
