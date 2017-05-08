/**
 * Machine Problem: Text Editor
 * CS 241 - Spring 2017
 */
// Author: Aneesh Durg
// Key_codes:g
// F_name:my_ext
// Description:"Interactively inserts text"
//---
#include <string.h>
void my_ext(Document *document, char **input_str, char **buffer, char k) {
  (void)input_str;
  (void)buffer;
  (void)k;
  (void)input_str;
  (*input_str) = interactive_cmd(document, 0, "Text to insert:");
  if ((*input_str)[strlen((*input_str)) - 1] == '\n')
    (*input_str)[strlen((*input_str)) - 1] = 0;
  refresh_display(document, 0, "");
}
