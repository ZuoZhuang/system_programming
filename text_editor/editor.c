/**
 * Machine Problem: Text Editor
 * CS 241 - Spring 2017
 */
#include "document.h"
#include "editor.h"
#include "format.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG 0

char *get_filename(int argc, char *argv[]) {
  // take a look at editor_main.c to see what this is used for
  return argv[1];
}

Document *handle_create_document(char *path_to_file) {
  return Document_create_from_file(path_to_file);;
}

void handle_cleanup(Document *document) {
  Document_destroy(document);
}

void handle_display_command(Document *document, size_t start_line,
                            size_t max_lines) {
  if (Document_size(document) == 0) {
      print_document_empty_error();
      return;
  }

  if (start_line <= 0 || max_lines <= 0) {
      return;
  }

  size_t end_line = (max_lines > Document_size(document) ? Document_size(document) : max_lines);
  while (start_line <= end_line)
      print_line(document, start_line++);

  return;
}

void handle_insert_command(Document *document, location loc, char *line) {
    /*while (Document_size(document) < loc.line_no) {*/
        /*Document_insert_line(document,Document_size(document)+1, "");*/
    /*}*/

    if (line == NULL) {line = "";}

    if (Document_size(document) < loc.line_no) {
        Document_insert_line(document, loc.line_no, "");
        /*if (DEBUG) {printf("!!this happened!!\n");}*/
    }

    const char *target_line = Document_get_line(document, loc.line_no);
    char *temp = (char *) malloc(strlen(target_line) + strlen(line) + 1);

    if (strlen(target_line) > loc.idx) {
        strncpy(temp, target_line, (size_t) loc.idx);
        temp[loc.idx] = '\0';
        strcat(temp, line);
        strcat(temp, target_line+loc.idx);
    }
    else if (strlen(target_line) == loc.idx) {
        strcpy(temp, target_line);
        strcat(temp, line);
    }
    else{
        //TODO what to do when it is over the length of the line;

        free(temp);
        temp = NULL;

        return;
    }

    Document_set_line(document, loc.line_no, temp);

    free(temp);
    temp = NULL;
}

void handle_delete_command(Document *document, location loc, size_t num_chars) {
    if (loc.line_no <= Document_size(document)) {
        const char *target_line = Document_get_line(document, loc.line_no);

        if (loc.idx <= strlen(target_line)) {
            /*size_t n = loc.idx < 1 ? 0 : loc.idx - 1;*/
            size_t n = loc.idx < 1 ? 0 : loc.idx;
            char *temp = (char *) malloc(strlen(target_line) - num_chars + 1);

            strncpy(temp, target_line, n);
            temp[n] = '\0';
            if (strlen(target_line) - loc.idx > num_chars) {
                strcat(temp, target_line+loc.idx+num_chars);
            }
            Document_set_line(document, loc.line_no, temp);

            free(temp);
            temp = NULL;
        }
    }
    else{
        //TODO handle deleting more lines than doc has
    }
}

void handle_delete_line(Document *document, size_t line_no) {
    if (line_no <= Document_size(document)) {
        Document_delete_line(document, line_no);
    }
    else {
        // TODO handle when line_no > document size
    }
}

location handle_search_command(Document *document, location loc,
                               const char *search_str) {
  // TODO implement handle_search_command
    size_t doc_size = Document_size(document);

    if (loc.line_no > 0 && loc.line_no <= doc_size && search_str != NULL) {
        size_t line_ptr = loc.line_no;

        const char *temp = Document_get_line(document, line_ptr);
        char *result_ptr = strstr(temp+loc.idx, search_str);

        if (result_ptr != NULL) {
            return (location){line_ptr, result_ptr - temp};
        }

        line_ptr++;

        while (line_ptr <= doc_size) {
            temp = Document_get_line(document, line_ptr);
            result_ptr = strstr(temp, search_str);

            if (result_ptr != NULL) {
                return (location){line_ptr, result_ptr - temp};
            }

            line_ptr++;
        }

        line_ptr = 1;

        while (line_ptr < loc.line_no) {
            temp = Document_get_line(document, line_ptr);
            result_ptr = strstr(temp, search_str);

            if (result_ptr != NULL) {
                return (location){line_ptr, result_ptr - temp};
            }

            line_ptr++;
        }
    }
    else {
        // TODO Handle line_no out of bound or search_str is NULL
    }
  return (location){0, 0};
}

void handle_merge_line(Document *document, size_t line_no) {
    if (line_no > 0 && line_no < Document_size(document)) {
        const char *line1 = Document_get_line(document, line_no);
        const char *line2 = Document_get_line(document, line_no+1);
        char *temp = (char *) malloc(strlen(line1) + strlen(line2) + 1);

        strcpy(temp, line1);
        strcat(temp, line2);
        Document_set_line(document, line_no++, temp);

        free(temp);
        temp = NULL;

        Document_delete_line(document, line_no);
    }
    else {
        // TODO handle invalid line_no
    }
}

void handle_split_line(Document *document, location loc) {
    if (loc.line_no > 0 && loc.line_no <= Document_size(document)) {
        const char *target_line = Document_get_line(document, loc.line_no);

        size_t append_size = strlen(target_line) >= loc.idx ? (strlen(target_line) - loc.idx) : 0;
        char *temp = (char *) malloc(append_size + 1);

        temp = (char *) malloc(append_size + 1);
        strcpy(temp, target_line+loc.idx);
        temp[append_size] = '\0';
        Document_insert_line(document, loc.line_no+1, temp);

        free(temp);

        temp = (char *) malloc(loc.idx + 1);
        strncpy(temp, target_line, loc.idx);
        temp[loc.idx] = '\0';
        Document_set_line(document, loc.line_no, temp);

        free(temp);
        temp = NULL;
    }
    else {
        // TODO handle invalid location
    }
}

void handle_save_command(Document *document, const char *filename) {
    if (filename != NULL) {
        Document_write_to_file(document, filename);
    }
    else {
        // TODO handle null filename
    }
}
