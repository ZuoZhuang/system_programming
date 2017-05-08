/**
 * Machine Problem: Text Editor
 * CS 241 - Spring 2017
 */
#pragma once
#include "document.h"
#include "tui.h"
// Author: Aneesh Durg
// Key_codes:g
// F_name:ext_0
// Description:"Interactively inserts text"
//---
#include <string.h>
void ext_0(Document *document, char **input_str, char **buffer, char k) {
  (void)input_str;
  (void)buffer;
  (void)k;
  (void)input_str;
  (*input_str) = interactive_cmd(document, 0, "Text to insert:");
  if ((*input_str)[strlen((*input_str)) - 1] == '\n')
    (*input_str)[strlen((*input_str)) - 1] = 0;
  refresh_display(document, 0, "");
}

// Author: Aneesh Durg
// Key_codes:b,y
// F_name:ext_1
// Description:"prints the word 'YAY!"
//---
#include <string.h>
void ext_1(Document *document, char **input_str, char **buffer, char k) {
  (void)document;
  (void)buffer;
  if (k == 'b') {
    asprintf(input_str, "YAY!");
    asprintf(&buffer[1], "Secret String");
  } else {
    if (buffer[1]) {
      *input_str = strdup(buffer[1]);
      free(buffer[1]);
      buffer[1] = NULL;
    }
  }
}
