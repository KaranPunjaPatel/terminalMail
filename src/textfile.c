#include "textfile.h"

void allocateTextfile(Textfile **textfile)
{
  *textfile = (Textfile *)malloc(sizeof(Textfile)); // Dereference `textfile` to set it correctly
  if (*textfile == NULL)
  {
    fprintf(stderr, "Fatal: failed to allocate %zu bytes.\n", sizeof(Textfile));
    abort();
  }

  (*textfile)->lines = (Line **)malloc(sizeof(Line *) * DEFAULT_NUMBER_OF_LINES);
  for (size_t i = 0; i < DEFAULT_NUMBER_OF_LINES; i++)
  {
    (*textfile)->lines[i] = NULL;
  }
  allocateLine(&(*textfile)->lines[0]);
  (*textfile)->max_count = DEFAULT_NUMBER_OF_LINES;
  (*textfile)->count = 1;
}

void deallocateTextfile(Textfile *textfile)
{
  for (size_t i = 0; i < textfile->count; i++)
  {
    deallocateLine(textfile->lines[i]);
  }
  free(textfile->lines);
  free(textfile);
}

void increaseTextfileLines(Textfile *textfile)
{
  textfile->max_count <<= 1;
  textfile->lines = (Line **)realloc(textfile->lines, sizeof(Line *) * textfile->max_count);
}

void printTextfile(Textfile *textfile)
{
  for (size_t i = 0; i < textfile->count; i++)
  {
    Line *line = textfile->lines[i];
    line->data[line->length] = '\0';
    printf("%s\r\n", line->data);
  }
}
