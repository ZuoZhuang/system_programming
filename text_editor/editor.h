/**
 * Machine Problem: Text Editor
 * CS 241 - Spring 2017
 */
#pragma once
#include "document.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

typedef struct {
  /* line_no can be any number >= 1 */
  size_t line_no;
  /**
   * if line_no <= Document_size(document)
   * then 0 <= idx <= strlen(Document_get_line(document, line_no))
   * else idx = 0
  */
  size_t idx;
} location;

/**
 * Note On Index:
 *
 * For all text editor commands line numbers are 1 indexed while characters
 * within a line are 0 indexed.
 * This means that if you are editing the following 1 line document:
 *
 * I love CS 241.
 *
 * And want to insert "really " into line 1 at character 2 then the result will
 * be:
 *
 * I really love CS 241.
 *
 * The user may only write to the begining, middle or end of a line.
 * Note that a user may write to a non existing line in which case you should
 * create the line.
 * If the user attempts to write to a non existing line, you may assume the user
 * will always insert at index 0.
*/

/**
 * This function gets the path_to_file from argc & argv (it's a one liner).
*/
char *get_filename(int argc, char *argv[]);

/**
 * This function opens a new document from a given path_to_file.
 * Use a function from document.h! Returns a pointer to the created document
 * This is also a one liner.
*/
Document *handle_create_document(char *path_to_file);

/**
 * Destroy the document. This is also a one liner.
*/
void handle_cleanup(Document *document);

/**
 * Prints out 'max_lines' lines of the document starting from line 'start_line'
 * or until the end of the document is reached.
*/
void handle_display_command(Document *document, size_t start_line,
                            size_t max_lines);

/**
 * Inserts 'line' into 'document' at 'loc'. See limits on locations in the
 * struct.
*/
void handle_insert_command(Document *document, location loc, char *line);

/**
 * Delete 'num_chars' characters from 'document' at 'loc' onwards
 * Note: if strlen(line) - idx <= num_chars
 * then delete all the characters from 'loc' to the end of the line.
*/
void handle_delete_command(Document *document, location loc, size_t num_chars);

/**
 * Deletes the 'line_no'th line from 'document'.
*/
void handle_delete_line(Document *document, size_t line_no);

/**
 * Returns the location of 'search_str' of 'document' starting at 'loc'.
 * If 'search_str' is not found then line number of the return location is 0.
 * Make sure that the returned location contains the found line number
 * as well as the character index at which the search string starts
*/
location handle_search_command(Document *document, location loc,
                               const char *search_str);

/**
 * Merges 'line_no'th line in document with it's following line.
 *
 * Note that this function may not be called on the last line of the document.
**/
void handle_merge_line(Document *document, size_t line_no);

/**
 * Splits the 'loc.line_no'th line in 'document' at the 'loc.idx'th character
 * into two parts.
 * The first part becomes the 'loc.line_no'th line in the document while the
 * second part gets inserted after the 'loc.line_no'th line shifting all the
 * subsequent lines down.
*/
void handle_split_line(Document *document, location loc);

/**
 * Has the document write itself to a file
 * with the specified 'path_to_file'
*/
void handle_save_command(Document *document, const char *path_to_file);
