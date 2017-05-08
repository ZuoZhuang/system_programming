/**
 * Machine Problem: Text Editor
 * CS 241 - Spring 2017
 */
#include "document.h"
#include "editor.h"
#include "format.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * You can programatically test your text editor.
*/
int main() {
  // Setting up a docment based on the file named 'filename'.

  location loc;
  loc.line_no = 1;
  loc.idx = 0;

  // test create
  char *filename = "test.txt";
  Document *document = Document_create_from_file(filename);

  // ---- handle_insert_command test ----

  // test insert at begining
  handle_insert_command(document, loc, "Hello world!");

  loc.line_no = 2;
  handle_insert_command(document, loc, "this is a test");

  // test insert at other place
  loc.idx = 3;
  handle_insert_command(document, loc, " \"add here\" ");


  //test insert over line size
  loc.line_no = 5;
  loc.idx = 0;
  handle_insert_command(document, loc, " \"test over line number\" ");

  // test save command
  handle_save_command(document, "test");

  // test insert over idx
  loc.line_no = 1;
  loc.idx = 100;
  handle_insert_command(document, loc, " test over idx ");

  // test insert equals idx
  loc.line_no = 6;
  loc.idx = 0;
  handle_insert_command(document, loc, "this");
  loc.idx = 4;
  handle_insert_command(document, loc, " is a test");
  handle_save_command(document, "test");

  // ---- handle_delete_command test ----
  // test delete one character

  loc.line_no = 2;
  loc.idx = 5;
  handle_delete_command(document, loc, 2);
  handle_save_command(document, "test");

  // test delete from end

  loc.line_no = 7;
  loc.idx = 0;
  handle_insert_command(document, loc, "this");
  loc.idx = 3;
  handle_delete_command(document, loc, 1);
  handle_save_command(document, "test");



  // print the first 1000 lines
  /*handle_display_command(document, 1, 2);*/
  /*handle_display_command(document, 0, 0);*/
  /*handle_display_command(document, 1, 0);*/
  /*handle_display_command(document, 0, 1);*/
  /*handle_display_command(document, 1, 10000000);*/
  /*handle_display_command(document, 100000, 1);*/
  /*handle_display_command(document, 1000000, 0);*/
  handle_display_command(document, 0, 1000000);


}
