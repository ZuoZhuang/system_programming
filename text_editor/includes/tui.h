/**
 * Machine Problem: Text Editor
 * CS 241 - Spring 2017
 */

// This file is responsible for controling the tui

#pragma once
#include <stdio.h>
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

/*
 * Constants and Global Variables
 */

enum escape_codes {
  up = 'A',
  down = 'B',
  left = 'D',
  right = 'C',
  delete = '3',
  home = 'H',
  end = 'F',
};

// Keycodes
#define ESC 27
#define DEL 126
#define BACKSPACE 127
#define CTRLF 6
#define CTRLW 23
#define CTRLX 24
#define CTRLA 1

// various constants
#define WHITESPACE 8
#define TABSPACE 4
#define TABSTR "    "

#define SCROLLUP 2
#define SCROLLDOWN 1

// global variables
size_t _cols = WHITESPACE;
size_t _line_num = 0;

size_t _max_lines = 20;
size_t _start_line = 1;

location _search_line_buffer = (location){0, 0};
char _scroll_flag = 0;

/*
 * Linux Terminal Commands
 *
 * References:
 * 1. http://www.termsys.demon.co.uk/vtansi.htm#keyboard
 * 2. https://en.wikipedia.org/wiki/ANSI_escape_code
 *
 * \033: stands for <ESC>
 *
 * Examples:
 * printf("\033[XA"); // Move up X lines;
 * printf("\033[XB"); // Move down X lines;
 * printf("\033[XC"); // Move right X columns;
 * printf("\033[XD"); // Move left X columns;
 * printf("\033[2J"); // Clear Screen
 */

/*
 * Cursors Control
 */
// Use console codes to move up n lines
void move_cursor_up_n(size_t n) { printf("\033[%zuA", n); }

// Use console codes to move down n lines
void move_cursor_down_n(size_t n) { printf("\033[%zuB", n); }

// use console codes to move right/forward n columns
void move_cursor_right_n(size_t n) { printf("\033[%zuC", n); }

// Use console codes to move left/backward n columns
void move_cursor_left_n(size_t n) { printf("\033[%zuD", n); }

// Use console codes to move to specific position (x,y)
void move_cursor_to(size_t x, size_t y) { printf("\033[%zu;%zuH", x, y); }

// Save current cursor position
void save_cursor_pos() { printf("\033[s"); }

// Restore cursor position after a save cursor
void restore_cursor_pos() { printf("\033[u"); }

// Restore cursor position in terms of collumns
void restore_cols() { printf("\r\033[%zuC", _cols); }

// Move up 1 line
void move_cursor_up() { move_cursor_up_n(1); }

// Move down 1 line
void move_cursor_down() { move_cursor_down_n(1); }

// Move left 1 line
void move_cursor_left() { move_cursor_left_n(1); }

// Move right 1 line
void move_cursor_right() { move_cursor_right_n(1); }

/*
 * Erasing Text
 */

// Erase from current cursor position to the end of the line
void erase_till_end() { printf("\033[K"); }

// Erase from current cursor position to the start of the line
void erase_from_start() { printf("\033[1K"); }

// Erase the entire line
void erase_line() { printf("\033[2K"); }

// Erase the entire screen
void erase_screen() { printf("\033[2J"); }

// reset the current line
void reset_line() { erase_line(); }

/*
 * Display Functions
 */
void refresh_display(Document *document, char new_line_flag, const char *msg) {
  size_t curr_size = Document_size(document);
  if (!curr_size) {
    move_cursor_up_n(_max_lines);
    printf("\n\n");
    handle_display_command(document, 1, 1);
    move_cursor_up();
    printf("\r");
    return;
  }
  printf("\r");

  if (_line_num > 1 && _line_num != _start_line) {
    move_cursor_up_n(_line_num - _start_line);
  }
  if (_scroll_flag == SCROLLUP)
    move_cursor_down();

  if (new_line_flag) {
    if (_line_num > _start_line + _max_lines - 1) {
      _start_line++;
      _scroll_flag = SCROLLDOWN;
    } else {
      move_cursor_down();
    }
    reset_line();
    curr_size--;
  }

  for (size_t i = 0; i <= _max_lines; i++) {
    reset_line();
    printf("\n");
  }

  reset_line();
  printf("%s\r", msg);
  if (_scroll_flag == SCROLLDOWN)
    move_cursor_up_n(_max_lines);
  else
    move_cursor_up_n(_max_lines + 1);

  _scroll_flag = 0;
  if (_search_line_buffer.line_no) {
    _start_line = _search_line_buffer.line_no;
    _line_num = _search_line_buffer.line_no;
    _cols = _search_line_buffer.idx + WHITESPACE;
    _search_line_buffer.line_no = 0;
  }
  handle_display_command(document, _start_line, _max_lines);
  size_t n = MIN(_max_lines, Document_size(document) - _start_line + 1);
  reset_line();
  if (n)
    move_cursor_up_n(n);
  if (_line_num > 1 && _line_num != _start_line)
    move_cursor_down_n(_line_num - _start_line);
  restore_cols();
}

char *interactive_cmd(Document *document, char new_line_flag,
                      const char *prompt) {
  size_t curr_size = Document_size(document);
  if (!curr_size) {
    return NULL;
  }
  printf("\r");

  if (_line_num > 1 && _line_num != _start_line) {
    move_cursor_up_n(_line_num - _start_line);
  }

  if (new_line_flag) {
    curr_size -= 1;
  }

  for (size_t i = 0; i <= _max_lines; i++) {
    printf("\r\n");
  }

  char *ret_str = NULL;
  size_t bufsize = 0;
  reset_line();
  printf("%s", prompt);
  system("stty sane");
  getline(&ret_str, &bufsize, stdin);
  system("stty cbreak");
  printf("\r");

  move_cursor_up_n(_max_lines + 1);
  handle_display_command(document, _start_line, _max_lines);
  size_t n = MIN(_max_lines, Document_size(document) - _start_line);
  reset_line();
  if (n)
    move_cursor_up_n(n);
  if (_line_num > 1)
    move_cursor_down_n(_line_num - _start_line);

  restore_cursor_pos();
  restore_cols();
  refresh_display(document, new_line_flag, "");
  return ret_str;
}
