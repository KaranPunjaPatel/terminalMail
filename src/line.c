#include "line.h"



void allocateLine(Line **line)
{
  *line = (Line *)malloc(sizeof(Line));
  if (*line == NULL)
  {
    fprintf(stderr, "Fatal: failed to allocate %zu bytes.\n", sizeof(Line));
    abort();
  }
  (*line)->data = (char *)malloc(sizeof(char) * DEFAULT_CHARS_PER_LINE);
  if ((*line)->data == NULL)
  {
    fprintf(stderr, "Fatal: failed to allocate %zu bytes.\n", sizeof(char) * DEFAULT_CHARS_PER_LINE);
    abort();
  }
  (*line)->capacity = DEFAULT_CHARS_PER_LINE;
  (*line)->length = 0;
}

void deallocateLine(Line *line)
{
  free(line->data);
  free(line);
  line = NULL;
}

void increaseLineSize(Line *line)
{
  line->capacity <<= 1;
  line->data = (char *)realloc(line->data, sizeof(char) * line->capacity);
}
