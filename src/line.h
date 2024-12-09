#ifndef LINE_H
#define LINE_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define DEFAULT_CHARS_PER_LINE 128

typedef struct
{
  size_t length;
  size_t capacity;
  char *data;
} Line;

void allocateLine(Line **line);

void deallocateLine(Line *line);

void increaseLineSize(Line *line);

#endif