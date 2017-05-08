/**
 * Machine Problem: Text Editor
 * CS 241 - Spring 2017
 */
#include "document.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_usage_error() {
  printf("\n  editor <filename> [window size=20]\n\n");
}

void print_line(Document *document, size_t index) {
  char *line = (char *)Document_get_line(document, index);
  if (line) {
    printf("%zu\t%s\n", index, line);
  } else {
    printf("%zu\n", index);
  }
}

void print_document_empty_error() {
  fprintf(stderr, "This file has no lines to display!\n");
}
