/**
 * Machine Problem: Text Editor
 * CS 241 - Spring 2017
 */
#pragma once
#include "extdefs.h"
#define EXTENSIONS(c)                                                          \
  ({                                                                           \
    case 'g': {                                                                \
      ext_0(document, &input_str, buffer, 'g');                                \
    } break;                                                                   \
    case 'b': {                                                                \
      ext_1(document, &input_str, buffer, 'b');                                \
    } break;                                                                   \
    case 'y': {                                                                \
      ext_1(document, &input_str, buffer, 'y');                                \
    } break;                                                                   \
  })