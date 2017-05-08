/**
 * Machine Problem: Text Editor
 * CS 241 - Spring 2017
 */
#pragma once
#include <stdio.h>

/**
 * This library will handle all the formating
 * for your document and text editor.
 *
 * Please use this to ensure that your formatting
 * matches what the autograder expects.
*/

/**
 * Call this function when your user has incorrectly
 * executed your text editor.
 */
void print_usage_error();

/**
 * Prints out one line from 'document'.
 */
void print_line(Document *document, size_t index);

/**
 * Error message that should be displayed if the document is empty and the user
 * attempts to display somethin.
 */
void print_document_empty_error();
