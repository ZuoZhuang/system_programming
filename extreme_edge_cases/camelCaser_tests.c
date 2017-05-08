/**
 * Extreme Edge Cases Lab
 * CS 241 - Spring 2017
 */
#include "camelCaser_tests.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "camelCaser.h"

/*
 * Compare whether two array is the same.
 *
 * @param  result   A pointer points to result array that is being test.
 * @param  target   A pointer points to right result
 * @return              Correctness of the program (0 for wrong, 1 for correct).
 */
int arrcmp (char **result, char **target) {
    while(*target != NULL ) {
        if (*result != NULL) {
            if (strcmp(*result++, *target++))
                return 0;
        }
        else{
            return 0;
        }
    }

    if(*result != NULL) {
        return 0;
    }

    return 1;
}

/*
 * Testing function for various implementations of camelCaser.
 *
 * @param  camelCaser   A pointer to the target camelCaser function.
 * @return              Correctness of the program (0 for wrong, 1 for correct).
 */
int test_camelCaser(char **(*camelCaser)(const char *)) {
  // TODO: Return 1 if the passed in function works properly; 0 if it doesn't.

  // Test 0
  const char * t0 = "Test for incomplete sente";
  char * r0[] = {NULL};
  char ** c0 = camelCaser(t0);
  if (!arrcmp(c0, r0))
      return 0;
  free(c0);
  c0 = NULL;

  // Test 1
  const char * t1 = "This is just a Test caSe. And it is a simple case. Which will not be wrong";
  char * r1[] = {"thisIsJustATestCase", "andItIsASimpleCase", NULL};
  char ** c1 = camelCaser(t1);
  if (!arrcmp(c1, r1))
      return 0;
  free(c1);
  c1 = NULL;

  // Test 2
  const char * t2 = "";
  char * r2[] = {NULL};
  char ** c2 = camelCaser(t2);
  if (!arrcmp(c2, r2))
      return 0;
  free(c2);
  c2 = NULL;

  // Test 3
  const char * t3 = "1This is a test of digits which is in the first word.";
  char * r3[] = {"1thisIsATestOfDigitsWhichIsInTheFirstWord", NULL};
  char ** c3 = camelCaser(t3);
  if (!arrcmp(c3, r3))
      return 0;
  free(c3);
  c3 = NULL;

  // Test 4
  const char * t4 = "1 This is a test of digits which is the first character.";
  char * r4[] = {"1ThisIsATestOfDigitsWhichIsTheFirstCharacter", NULL};
  char ** c4 = camelCaser(t4);
  if (!arrcmp(c4, r4))
      return 0;
  free(c4);
  c4 = NULL;

  // Test 5
  const char * t5 = "This is a test of digits which is 1 body word.";
  char * r5[] = {"thisIsATestOfDigitsWhichIs1BodyWord", NULL };
  char ** c5 = camelCaser(t5);
  if (!arrcmp(c5, r5))
      return 0;
  free(c5);
  c5 = NULL;


  // Test 6
  const char * t6 = "This is a test of digits which is in 1body word.";
  char * r6[] = {"thisIsATestOfDigitsWhichIsIn1BodyWord", NULL };
  char ** c6 = camelCaser(t6);
  if (!arrcmp(c6, r6))
      return 0;
  free(c6);
  c6 = NULL;

  // Test 7
  const char * t7 = "...";
  char * r7[] = {"", "", "", NULL };
  char ** c7 = camelCaser(t7);
  if (!arrcmp(c7, r7))
      return 0;
  free(c7);
  c7 = NULL;

  // Test 8
  const char * t8 = "This is a test for three ... in a sentence.";
  char * r8[] = {"thisIsATestForThree", "", "", "inASentence", NULL };
  char ** c8 = camelCaser(t8);
  if (!arrcmp(c8, r8))
      return 0;
  free(c8);
  c7 = NULL;

  // Test 9
  const char * t9 = "This is a test for three   in a sentence.";
  char * r9[] = {"thisIsATestForThreeInASentence", NULL };
  char ** c9 = camelCaser(t9);
  if (!arrcmp(c9, r9))
      return 0;
  free(c9);
  c9 = NULL;

   /*Test 10*/
  const char * t10 = "This is a test of \n\t.";
  char * r10[] = {"thisIsATestOf", NULL };
  char ** c10 = camelCaser(t10);
  if (!arrcmp(c10, r10))
      return 0;
  free(c10);
  c10 = NULL;

  // Test 11
  const char * t11 = "?..xyz..";
  char * r11[] = {"", "", "", "xyz", "", NULL};
  char ** c11 = camelCaser(t11);
  if (!arrcmp(c11, r11))
      return 0;
  free(c11);
  c11 = NULL;

  // Test 12
  const char * t12 = "This is a test of @ in sentence.";
  char * r12[] = {"thisIsATestOf", "inSentence", NULL };
  char ** c12 = camelCaser(t12);
  if (!arrcmp(c12, r12))
      return 0;
  free(c12);
  c12 = NULL;

  // Test 13
  const char * t13 = "@This is shit.";
  char * r13[] = {"", "thisIsShit", NULL };
  char ** c13 = camelCaser(t13);
  if (!arrcmp(c13, r13))
      return 0;
  free(c13);
  c13 = NULL;


  // Test 14
  const char * t14 = "\\";
  char * r14[] = {"", NULL };
  char ** c14 = camelCaser(t14);
  if (!arrcmp(c14, r14))
      return 0;
  free(c14);
  c14 = NULL;

  // Test 15
  const char * t15 = "I don't know what 2 test now.";
  char * r15[] = {"iDon","tKnowWhat2TestNow", NULL };
  char ** c15 = camelCaser(t15);
  if (!arrcmp(c15, r15))
      return 0;
  free(c15);
  c15 = NULL;

  return 1;
}
