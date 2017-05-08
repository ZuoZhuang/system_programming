/**
 * Machine Problem: Text Editor
 * CS 241 - Spring 2017
 */
// Author: Aneesh Durg
// Key_codes:b,y
// F_name:my_ext
// Description:"prints the word 'YAY!"
//---
#include <string.h>
void my_ext(Document *document, char **input_str, char **buffer, char k) {
  (void)document;
  (void)buffer;
  if (k == 'b') {
    asprintf(input_str, "YAY!");
    asprintf(&buffer[EXT_NO], "Secret String");
  } else {
    if (buffer[EXT_NO]) {
      *input_str = strdup(buffer[EXT_NO]);
      free(buffer[EXT_NO]);
      buffer[EXT_NO] = NULL;
    }
  }
}