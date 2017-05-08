/**
 * Parallel Make
 * CS 241 - Spring 2017
 */

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "callbacks.h"
#include "compare.h"
#include "graph.h"
#include "parser.h"
#include "rule.h"
#include "vector.h"

/**
 * Creates dynamically sized array of tokens from string. The strings
 * themselves are NOT dynamically allocated, however.
 * Tokens are separated by spaces ;)
 */
static char **parse_line(char *line) {
  assert(line != NULL);
  size_t space_count = 0;
  for (char *ptr = line; *ptr; ++ptr)
    if (*ptr == ' ')
      ++space_count;
  char **tokens = malloc((2 + space_count) * sizeof(char *));
  size_t tokIdx = 0;
  while (1) {
    tokens[tokIdx] = strtok(line, " ");
    line = NULL;
    if (!tokens[tokIdx])
      break;
    if (*tokens[tokIdx]) // If strtok gives empty string, don't increment idx
      ++tokIdx;
  };
  return realloc(tokens, sizeof(char *) * (tokIdx + 1));
}

/**
 * Uses djb2 string hashing algorithm to hash a string.
 * It essentially performs the following hash
 *   hash' s (c:cc) = hash (33*s + c) cc
 *   hash' s [] = s
 *   hash = hash' 5381
 *
 * http://www.cse.yorku.ca/~oz/hash.html
 */
static size_t str_hash_function(void *s) {
  char const *str = (char const *)s;
  size_t hash = 5381;
  int c;

  while ((c = *str++))
    hash = (hash << 5) + hash + c;

  return hash;
}

/**
 * Wrapper for strcmp() that fits the `compare` template.
 */
static int strcmpv(void *a, void *b) { return strcmp((char *)a, (char *)b); }

/**
 * Wrapper for strdup() that fits the `copy_constructor` template.
 */
static void *strdupv(void *s) { return (void *)strdup((char *)s); }

/**
 * Wrapper for free() that fits the `destructor` template.
 */
static void strdestroy(void *s) { free(s); }

/**
 * Initializes a rule with a given target name.
 */
static void *str_to_rule_constructor(void *target_name) {
  rule_t *r = malloc(sizeof(rule_t));
  rule_init(r);
  r->target = strdupv(target_name);
  return r;
}

static void rule_destroy_v(void *r) {
  rule_t *rule = (rule_t *)r;
  rule_destroy(rule);
}

static char *skip_whitespace(char const *str) {
  while (*str)
    if (!isspace(*(str++)))
      return (char *)(str - 1);
  return (char *)str;
}

static char *find_whitespace(char const *str) {
  while (*str)
    if (isspace(*(str++)))
      return (char *)(str - 1);
  return (char *)str;
}

static char *terminate_whitespace(char *str) {
  char *search = find_whitespace(str);
  *search = '\0';
  return search;
}

static char *strip_back_whitespace(char *str) {
  size_t len = strlen(str);
  char *back = str + len - 1;
  while (back >= str && isspace(*back)) {
    *back-- = '\0';
  }
  return str;
}

graph *parser_parse_makefile(const char *makeFileName, char **run_targets) {
  // Open the makefile
  FILE *f = fopen(makeFileName, "r");
  if (!f) {
    fprintf(stderr, "make: %s: No such file or directory", makeFileName);
    exit(2);
  }
  graph *dependency_graph =
      graph_create(str_hash_function, strcmpv, strdupv, strdestroy,
                   str_to_rule_constructor, rule_destroy_v, NULL, NULL);
  // capture first rule name in case user did not specify build targets
  char *first_target = NULL;
  rule_t *curr_rule = NULL;

  char *lineBuf = NULL;
  size_t bytes = 0;
  ssize_t len = 0;
  size_t line_number = 0;

  while ((len = getline(&lineBuf, &bytes, f)) != -1) {
    ++line_number;
    if (len && lineBuf[len - 1] == '\n') {
      lineBuf[--len] = '\0';
      if (len && lineBuf[len - 1] == '\r')
        lineBuf[--len] = '\0';
    }
    strip_back_whitespace(lineBuf);
    char *line = skip_whitespace(lineBuf);
    if (!(*line))
      continue;
    if (lineBuf[0] == '\t' && first_target) {
      // Recipe line
      if (!curr_rule) {
        fprintf(stderr, "%s:%zu: *** recipe commences before first target.  "
                        "Stop.\n",
                makeFileName, line_number);
        exit(2);
      }
      vector_push_back(curr_rule->commands, line);
      // Found start of new rule line?
    } else if (isalnum(line[0])) {
      curr_rule = NULL;
      // Find first colon to split on
      char *depLine = strstr(line, ":");
      // Did we actually find it?
      if (!depLine) {
        fprintf(stderr, "%s:%zu: *** missing separator.  Stop.\n", makeFileName,
                line_number);
        exit(2);
      }

      // Remove the colon, and setup target name and dependency string
      depLine[0] = '\0';
      ++depLine;
      // Remove trailing whitespace and other nonsense
      terminate_whitespace(line);

      // We found a new rule, so let's push this to the dependency graph
      if (!graph_contains_vertex(dependency_graph, line)) {
        // Found a completely new rule, so we must add it to the graph
        graph_set_vertex_value(dependency_graph, line, line);
        // In case the user doesn't manually specify build targets
        if (!first_target)
          first_target = strdupv(line);
      }

      curr_rule = graph_get_vertex_value(dependency_graph, line);
      char **dependencies = parse_line(depLine);

      for (char **depPtr = dependencies; *depPtr; ++depPtr) {
        // We found a new rule, so push it to the dependency graph
        if (!graph_contains_vertex(dependency_graph, *depPtr)) {
          graph_set_vertex_value(dependency_graph, *depPtr, *depPtr);
        }
        // Create a dependency edge
        graph_add_edge(dependency_graph, line, *depPtr);
      }
      free(dependencies);
    } else {
      fprintf(stderr, "%s:%zu: *** missing separator.  Stop.\n", makeFileName,
              line_number);
      exit(2);
    }
  }
  free(lineBuf);
  // If no build targets specified, build the first rule ever defined
  char *default_targets[] = {first_target, NULL};
  if (!run_targets || !(*run_targets)) {
    if (!first_target) {
      fprintf(stderr, "make: *** No targets.  Stop.\n");
      exit(2);
    }
    run_targets = default_targets;
  }
  graph_set_vertex_value(dependency_graph, "", "");
  for (char **targetPtr = run_targets; *targetPtr; ++targetPtr) {
    if (!graph_contains_vertex(dependency_graph, *targetPtr)) {
      fprintf(stderr, "parmake: *** No rule to make target '%s'.  Stop.\n",
              *targetPtr);
      exit(1);
    } else {
      graph_add_edge(dependency_graph, "", *targetPtr);
    }
  }
  free(first_target);
  fclose(f);
  return dependency_graph;
}
