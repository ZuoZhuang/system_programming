/**
 * Pointers Gone Wild
 * CS 241 - Spring 2017
 */

#include "part2-functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * (Edit this function to print out the "Illinois" lines in
 * part2-functions.c in order.)
 */
int main() {
  // your code here

  // First
  first_step(81);

  // Second
  int value = 132;
  second_step(&value);

  // Third
  value = 8942;
  int *p3 = &value;
  int **ptr3 = &p3;
  double_step(ptr3);

  // Forth
  value = 15;
  char *str4 = (char *)malloc(sizeof(char)*5 + sizeof(int *)*2);
  sprintf(str4, "%s%p", "abcde", &value);
  *(int *)(str4 + 5) = 15;
  strange_step(str4);
  free(str4);
  str4 = NULL;

  // Fifth
  char *a5 = "abc";
  empty_step((void *)a5);

  // Sixth
  char *s6 = "abcu";
  two_step((void *)s6, s6);

  // Seventh
  char *s7 = "This is like shit";
  three_step(s7, s7+2, s7+4);

  // Eighth
  char *s8 = "abcdefghijklmnopqrstuvwxyz";
  char *s81 = "ghjklmnopqrstuvwxyz";
  char *s82 = "ghprstuvwxyz";
  step_step_step(s8, s81, s82);

  // Ninth
  char *a9 = "abcde";
  int b9 = 97;
  it_may_be_odd(a9, b9);

  // Tenth
  char a10[20] = " ,CS241";
  tok_step(a10);

  // Eleventh
  char *blue = (char *) malloc(sizeof(char)*10);
  sprintf(blue, "%c%2c%c", 1, 0, 3);
  char *orange;
  orange = blue;
  the_end((void *)orange, (void *)blue);

  free(blue);
  blue = NULL;

  return 0;
}
