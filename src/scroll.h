#ifndef SCROLL_H
#define SCROLL_H

#include "cursor.h"
#include <stdint.h>

typedef struct
{
  size_t start_line; // Line to start displaying from
  size_t start_col;  // Column to start displaying from (horizontal scroll)
  size_t max_lines;  // Number of lines the terminal can display
  size_t max_width;  // Number of characters per line the terminal can display
} TextScroll;

void checkPointerScroll(Cursor *pointer, TextScroll *scroll);

#endif