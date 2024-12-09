#ifndef TEXTFILE_H
#define TEXTFILE_H

#include <stdio.h>
#include <stdlib.h>
#include "line.h"

#define DEFAULT_NUMBER_OF_LINES 8

typedef struct
{
  size_t count;
  size_t max_count;
  Line **lines;
} Textfile;

void allocateTextfile(Textfile **textfile);

void deallocateTextfile(Textfile *textfile);

void increaseTextfileLines(Textfile *textfile);

void printTextfile(Textfile *textfile);

#endif