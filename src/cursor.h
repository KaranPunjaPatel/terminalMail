#ifndef CURSOR_H
#define CURSOR_H

#include <stdlib.h>

typedef struct
{
  size_t line_count;
  size_t character_count;
} Cursor;

#endif