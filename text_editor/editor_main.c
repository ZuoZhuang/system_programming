/**
 * Machine Problem: Text Editor
 * CS 241 - Spring 2017
 */
#include "document.h"
#include "editor.h"
#include "exts/extensions.h"
#include "format.h"
#include "tui.h"

#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

short load_extensions = 0;
char *buffer[100];

/**
 * This is the will be the entry point to your text editor.
*/
int entry_point(int argc, char *argv[]) {

  // Checking to see if the editor is being used correctly.
  if (argc < 2) {
    print_usage_error();
    return 1;
  }

  // Checking to see if a custom window size was specified
  if (argc >= 3) {
    _max_lines = MAX(20, (size_t)(atoi(argv[2])));
  }

  // Setting up a document based on the file named 'filename'.
  char *filename = get_filename(argc, argv);
  Document *document = handle_create_document(filename);

  if (document == NULL) {
    fprintf(stderr, "Failed to create document!\n");
    return 1;
  }

  _line_num = 1;

  printf("Welcome to cs241 text-editor!\n");

  char *scroll_msg = "Press the down arrow key to see more lines";
  refresh_display(document, 0, scroll_msg);
  restore_cols();
  char new_line_flag = 0;

  while (1) {
    save_cursor_pos();
    char c = getchar();
    char *input_str = NULL;
    // subtracting WHITESPACE to index to account for line number display
    size_t idx = _cols - WHITESPACE;

    if (c == '\n') {
      printf("\n");
      move_cursor_up();
      if (Document_size(document)) {
        size_t l = strlen(Document_get_line(document, _line_num));
        if (_line_num != Document_size(document) || idx != l) {
          handle_split_line(document, (location){_line_num, idx});
          _line_num++;
          if (_line_num > _start_line + _max_lines - 1) {
            _start_line++;
            move_cursor_up();
          }
          _cols = WHITESPACE;
          refresh_display(document, new_line_flag, "");
          continue;
        }
      }

      _line_num++;

      // setting flag to tell refresh_display to add a line
      new_line_flag = _line_num > Document_size(document);

      // setting character to 0 (adds an empty newline)
      c = 0;
      // reseting _cols to WHITESPACE-1 , (adding a character will cause another
      // +1 offset)
      // so by the end of the loop _cols will be correctly reset to WHITESPACE
      _cols = WHITESPACE - 1;
    } else if (c == ESC) {

      // OPTIONAL: Extending the TUI's functionality

      // the following allow keys that call the escape sequence (^[[) to do
      // things
      // (e.g. arrow keys)
      // you can add your own keyboard shortcuts here! To do this, add a case
      // for a
      // character (for example, 'x') then you can call your shortcut with (Esc
      // + [ + x)
      // if you want to replace the text being written to the document, make a
      // heap allocated
      // string and assign it to (the above declared) variable input_str.

      // current line number is saved in _line_num (declared in includes/tui.h)
      // and collumn number in _cols (declared above) respectively.

      // to change the cursor position it's best to place a location struct in
      // the
      // variable _search_line_buffer (declared in includes/tui.h)
      char code = getchar();
      if (code != '[') {
        if (load_extensions) {
          switch (code) {
            EXTENSIONS(code);
          default: {
            refresh_display(document, new_line_flag, "Invalid keycode!");
            continue;
          }
          }
        } else {
          refresh_display(document, new_line_flag, "Invalid escape sequence!");
          continue;
        }
      } else if (Document_size(document)) {
        code = getchar();
        switch (code) {
        case up: {
          _line_num--;
          if (1 > _line_num) {
            restore_cursor_pos();
            _line_num = 1;
          } else {
            if (_line_num < _start_line) {
              _start_line--;
              _scroll_flag = SCROLLUP;
            }
            move_cursor_up();
            idx = MIN(idx, strlen(Document_get_line(document, _line_num)));
            _cols = idx + WHITESPACE;
          }
        } break;
        case down: {
          _line_num++;

          if (_line_num > Document_size(document)) {
            restore_cursor_pos();
            _line_num = Document_size(document);
          } else {
            if (_line_num > _start_line + _max_lines - 1) {
              _start_line++;
              _scroll_flag = SCROLLDOWN;
              move_cursor_up();
            } else
              move_cursor_down();
            idx = MIN(idx, strlen(Document_get_line(document, _line_num)));
            _cols = idx + WHITESPACE;
          }
        } break;
        case left: {
          if (idx > 0) {
            _cols--;
          }
        } break;
        case right: {
          if (idx + 1 <= strlen(Document_get_line(document, _line_num))) {
            _cols++;
          }
        } break;
        case delete:
          // allow the character code to be caught instead
          break;
        case home: {
          _cols = WHITESPACE;
        } break;
        case end: {
          _cols = WHITESPACE + strlen(Document_get_line(document, _line_num));
        } break;
        default: {
          refresh_display(document, new_line_flag, "Invalid keycode!");
          continue;
        }
        }
      } else {
        restore_cursor_pos();
        restore_cols();
      }
      if (!input_str) {
        refresh_display(document, new_line_flag, "");
        continue;
      }
    } else if (c == CTRLX) {
      // save with ctrl + x
      handle_save_command(document, filename);
      reset_line();
      char *msg;
      asprintf(&msg, "Wrote file to: %s", filename);
      refresh_display(document, new_line_flag, msg);
      free(msg);
      continue;
    } else if (c == CTRLW) {
      // delete line with ctrl + w
      handle_delete_line(document, _line_num);
      if (_line_num > Document_size(document)) {
        if (Document_size(document) != 0) {
          move_cursor_up();
          _line_num--;
        }
      }
      char *msg = NULL;
      asprintf(&msg, "Deleted line: %zu", _line_num);
      refresh_display(document, new_line_flag, msg);
      free(msg);
      continue;
    } else if (c == CTRLA) {
      // Close file with ctrl + a
      handle_cleanup(document);
      exit(0);
    } else if (c == CTRLF) {
      // ctrl+f to search
      char *search_str =
          interactive_cmd(document, new_line_flag, "Enter Search String:");
      if (search_str) {
        if (search_str[strlen(search_str) - 1] == '\n') {
          search_str[strlen(search_str) - 1] = 0;
        }
        // offsetting idx by 1 to not return the same location incase the user
        // is already
        // on top of a search result
        _search_line_buffer = handle_search_command(
            document, (location){_line_num, idx + 1}, search_str);
        char *msg = NULL;
        if (_search_line_buffer.line_no) {
          asprintf(&msg, "Found %s at line: %zu char: %zu", search_str,
                   _search_line_buffer.line_no, _search_line_buffer.idx);
        } else {
          asprintf(&msg, "%s not found!", search_str);
        }
        refresh_display(document, new_line_flag, msg);
        free(msg);
        free(search_str);
      }
      continue;
    }
    // if it passed all the ifs, it means it's time to actually edit the file!
    reset_line();
    restore_cursor_pos();
    if (c == BACKSPACE) {
      // bkspc mode
      if (idx >= 1) {
        handle_delete_command(document, (location){_line_num, idx - 1}, 1);
        _cols--;
        refresh_display(document, new_line_flag, "");
      } else if (_line_num != 1) {
        // cursor is on first collumn, so merge this line with the previous one
        size_t newidx = strlen(Document_get_line(document, _line_num - 1));
        handle_merge_line(document, _line_num - 1);
        move_cursor_up();
        _cols = newidx + WHITESPACE;
        restore_cols();
        _line_num--;
        if (_line_num < _start_line) {
          _start_line--;
          _scroll_flag = SCROLLUP;
        }
        char *msg = NULL;
        asprintf(&msg, "merged: %zu", _line_num);
        refresh_display(document, new_line_flag, msg);
        free(msg);
      } else {
        refresh_display(document, new_line_flag, "");
      }
      continue;
    }

    else if (c == DEL) {
      // delete mode
      if (idx != strlen(Document_get_line(document, _line_num))) {
        handle_delete_command(document, (location){_line_num, idx}, 1);
      } else if (_line_num != Document_size(document)) {
        // cursor is on last collumn, so merge this line with the next one
        handle_merge_line(document, _line_num);
        restore_cols();
      }
      char *msg = NULL;
      asprintf(&msg, "Deleted: %zu", idx + 1);
      refresh_display(document, new_line_flag, msg);
      free(msg);
      continue;
    } else {
      // insert mode
      if (input_str == NULL) {
        if (c == '\t') {
          // convert tabs to spaces
          asprintf(&input_str, TABSTR);
          _cols += TABSPACE;
        } else {
          // convert character to string
          asprintf(&input_str, "%c", c);
          _cols++;
        }
      } else {
        _cols += strlen(input_str);
      }
      handle_insert_command(document, (location){_line_num, idx}, input_str);
      free(input_str);
    }

    refresh_display(document, new_line_flag, "");
    new_line_flag = 0;
    continue;
  }
}

// the code below allows the editor to run in a
// seperate process so that the tty mode can be restored

size_t _lines = 20;

void cleanupAndExit(int sig) {
  (void)sig;
  // restore the normal stty behavior
  // (getchar requires enter again)
  system("stty sane");
  // move cursor down below the last possible
  // printed by the tui
  for (size_t i = 0; i <= _lines; i++)
    printf("\n");
  exit(0);
}

int main(int argc, char *argv[]) {
  // Catch ctrl+c

  // allow getchar() to read character by charcter
  // no need to press enter
  system("stty cbreak");
  // clear the screen
  system("tput clear");

  if (argc >= 3) {
    _lines = MAX(20, (size_t)(atoi(argv[2])));
  }

  if (!strcmp(argv[argc - 1], "--enable-exts")) {
    load_extensions = 1;
  }

  pid_t pid = fork();

  if (pid < 0) {
    // fail
    perror("Can't fork.");
  } else if (pid == 0) {
    // child
    entry_point(argc, argv);
    exit(0);
  }
  signal(SIGINT, cleanupAndExit);
  int status = 0;
  waitpid(pid, &status, 0);

  cleanupAndExit(0);
}
