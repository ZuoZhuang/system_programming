/**
 * Parallel Make
 * CS 241 - Spring 2017
 */

#include <assert.h>
#include <string.h>
#ifdef DEBUG
#include <signal.h>
#include <stdio.h>
#endif

#include "callbacks.h"
#include "rule.h"
#include "vector.h"

static void *zero_byte(void) { return calloc(1, 1); }

static void *strdupv(void *s) { return (void *)strdup((char *)s); }

static void strdestroy(void *s) { free(s); }

void rule_init(rule_t *rule) {
  assert(rule != NULL);
  rule->target = NULL;
  rule->commands = vector_create(strdupv, strdestroy, zero_byte);
  rule->state = 0;
  rule->data = NULL;
}

void rule_destroy(rule_t *rule) {
  if (!rule)
    return;
  strdestroy(rule->target); // Can free null :)
  vector_destroy(rule->commands);
  free(rule);
}

void rule_soft_copy(rule_t *dest, rule_t *src) {
  assert(src != NULL);
  memcpy(dest, src, sizeof(*src));
}
