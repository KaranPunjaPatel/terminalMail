#ifndef CURRENTFILEEDITOR_H
#define CURRENTFILEEDITOR_H

#include <stdint.h>
#include <stdlib.h>
#include <curses.h>
#include <string.h>
#include "textfile.h"
#include "cursor.h"
#include "scroll.h"

#define KEY_ALT 18
#define KEY_ESC 27
#ifndef CTRL
#define CTRL(c) ((c) & 037)
#endif

void displayPageName(const char *msg, TextScroll *scroll)
{
  // printf("Length: %lu\n", strlen(msg));
  int terminalWidth = scroll->max_width;
  size_t msgLength = strlen(msg);

  // Ensure enough memory for the corrected message and null-terminator
  char *correctedMsg = (char *)malloc(sizeof(char) * (terminalWidth + 1));
  if (!correctedMsg)
  {
    perror("Failed to allocate memory");
    return;
  }

  // Fill the entire string with `=` initially
  memset(correctedMsg, '=', terminalWidth);
  correctedMsg[terminalWidth] = '\0'; // Null-terminate

  // Add special characters at the beginning and end
  correctedMsg[0] = '/';
  correctedMsg[1] = '*';
  correctedMsg[(terminalWidth - msgLength) / 2 - 1] = ' ';
  correctedMsg[(terminalWidth - msgLength) / 2 + msgLength] = ' ';
  correctedMsg[terminalWidth - 2] = '*';
  correctedMsg[terminalWidth - 1] = '/';

  // Copy the message into the middle
  int startPos = (terminalWidth - msgLength) / 2;
  memcpy(correctedMsg + startPos, msg, msgLength);

  // Print the resulting string
  mvprintw(scroll->max_lines + 1, 0, "%s", correctedMsg);

  // Free the allocated memory
  free(correctedMsg);
}

void redrawScreen(Textfile *file, TextScroll *scroll, const char *type)
{
  clear();
  for (size_t i = 0; i < scroll->max_lines && (scroll->start_line + i) < file->count; i++)
  {
    Line *line = file->lines[scroll->start_line + i];
    if (line != NULL && line->length > scroll->start_col) // Check if there's text to display after `start_col`
    {
      // mvprintw(i, 0, "%.*s", (int)scroll->max_width, line->data + scroll->start_col);
      mvprintw(i, 0, "%.*s", (int)(line->length - scroll->start_col), line->data + scroll->start_col);
    }
  }
  attron(COLOR_PAIR(1));
  displayPageName(type, scroll);
  attroff(COLOR_PAIR(1));
  refresh();
}

void resetScreen(Textfile *file, Cursor *pointer, TextScroll *scroll, const char *type)
{
  redrawScreen(file, scroll, type);
  move(pointer->line_count - scroll->start_line, pointer->character_count - scroll->start_col);

  refresh();
}

void performOperation(int ch, Textfile *file, Cursor *pointer, TextScroll *scroll, const char *type)
{
  Line *curr_line = file->lines[pointer->line_count];
  switch (ch)
  {
  case KEY_ESC:
    break;

  case 37:
  case KEY_LEFT:
  {

    if (pointer->character_count == 0)
    {
      if (pointer->line_count == 0)
        break;

      pointer->line_count--;
      pointer->character_count = file->lines[pointer->line_count]->length;
    }
    else
      pointer->character_count--;

    break;
  }

  case 38:
  case KEY_UP:
  {
    if (pointer->line_count == 0)
    {
      scroll->start_line = 0;
      scroll->start_col = 0;
      pointer->character_count = 0;
    }
    else
    {
      pointer->line_count--;
    }

    if (pointer->character_count > file->lines[pointer->line_count]->length)
      pointer->character_count = file->lines[pointer->line_count]->length;

    // checkPointerScroll(&pointer, &scroll);

    // redrawScreen(file, &scroll); // Update screen
    // move(pointer->line_count - scroll->start_line, pointer->character_count - scroll->start_col);
    // move(pointer->line_count, pointer->character_count);
    break;
  }

  case 39:
  case KEY_RIGHT:
  {

    if (pointer->character_count == curr_line->length)
    {
      if (pointer->line_count + 1 != file->count)
      {
        pointer->line_count++;
        pointer->character_count = 0;
      }
    }
    else
      pointer->character_count++;

    if (pointer->character_count < curr_line->length)
    {
      pointer->character_count++;
      if (pointer->character_count >= scroll->start_col + scroll->max_width)
      {
        scroll->start_col = pointer->character_count - scroll->max_width + 1; // Scroll right
      }
    }

    // checkPointerScroll(&pointer, &scroll);

    // redrawScreen(file, &scroll);
    // move(pointer->line_count - scroll->start_line, pointer->character_count - scroll->start_col);
    break;
  }

  case 40:
  case KEY_DOWN:
  {
    if (pointer->line_count + 1 == file->count)
      pointer->character_count = curr_line->length;
    else
      pointer->line_count++;

    if (pointer->character_count > file->lines[pointer->line_count]->length)
      pointer->character_count = file->lines[pointer->line_count]->length;

    // checkPointerScroll(&pointer, &scroll);
    // redrawScreen(file, &scroll); // Update screen
    // move(pointer->line_count, pointer->character_count);
    // move(pointer->line_count - scroll->start_line, pointer->character_count - scroll->start_col);
    break;
  }

  case KEY_BACKSPACE:
  {
    if (pointer->character_count == 0)
    {
      if (pointer->line_count == 0)
      { // At the zero'th character so there is nothing to backspace
        curr_line = NULL;
        break;
      }

      Line *prev_line = file->lines[pointer->line_count - 1];
      // TODO: Join this line with previous line
      while (prev_line->length + curr_line->length > prev_line->capacity) // Increase line size till can fit previous line characters
      {
        increaseLineSize(prev_line);
      }

      if (curr_line->length > 0)
      {
        // Append the data to previous line
        memcpy((char *)prev_line->data + prev_line->length, curr_line->data, sizeof(char) * (curr_line->length));
        // Change the line length after append
        prev_line->length += curr_line->length;
      }
      // Free the line
      deallocateLine(curr_line);

      if (file->count > pointer->line_count + 1) // Remove this line pointer from the file
        memmove(&file->lines[pointer->line_count], &file->lines[pointer->line_count + 1], sizeof(Line *) * (file->count - pointer->line_count - 1));
      file->count--;

      pointer->line_count--;
      pointer->character_count = prev_line->length;
    }
    else if (pointer->character_count == curr_line->length)
    {
      curr_line->length--;
      pointer->character_count--;
    }
    else
    { // TODO: Shift all the next characters in line to the cursor-1 position
      memmove(curr_line->data + pointer->character_count - 1, curr_line->data + pointer->character_count, sizeof(char) * (curr_line->length - pointer->character_count));

      curr_line->length--;
      pointer->character_count--;
      // move(pointer->line_count - scroll->start_line, pointer->character_count - scroll->start_col);
      // delch();
    }
    break;
  }

  case '\n':
  case KEY_ENTER:
  {
    if (file->count + 1 >= file->max_count)
    { // Reached maximum lines possible
      increaseTextfileLines(file);
    }

    if (pointer->line_count + 1 != file->count)
      memmove(&file->lines[pointer->line_count + 2], &file->lines[pointer->line_count + 1], sizeof(Line *) * (file->count - pointer->line_count - 2));

    // Allocate line
    allocateLine(&file->lines[pointer->line_count + 1]);
    // Increment the lines count in file
    file->count++;

    // Copy the data to new line
    if (curr_line->length > pointer->character_count)
    {
      size_t copy_length = curr_line->length - pointer->character_count;
      memcpy((char *)file->lines[pointer->line_count + 1]->data, curr_line->data + pointer->character_count, sizeof(char) * (copy_length));
      curr_line->length = pointer->character_count;
      curr_line->data[pointer->character_count] = '\0';
      // Change the length to new length
      file->lines[pointer->line_count + 1]->length = copy_length;
    }

    // Move the cursor accordingly
    pointer->line_count++;
    pointer->character_count = 0;

    // move(pointer->line_count, pointer->character_count);
    // addch(13); // TODO: this or that
    // addch(10); // TODO: this or that
    // move(pointer->line_count, pointer->character_count);
    // move(pointer->line_count - scroll->start_line, pointer->character_count - scroll->start_col);

    break;
  }

  default:
  {
    if (curr_line->length >= curr_line->capacity)
    {
      // increase line size
      increaseLineSize(curr_line);
    }

    if (pointer->character_count == curr_line->length)
    {
      curr_line->data[pointer->character_count] = ch;
    }
    else
    {
      memmove((char *)curr_line->data + pointer->character_count + 1, curr_line->data + pointer->character_count, sizeof(char) * (curr_line->length - pointer->character_count));
      curr_line->data[pointer->character_count] = ch;
    }
    curr_line->length++;
    pointer->character_count++;
    // move(pointer->line_count - scroll->start_line, pointer->character_count - scroll->start_col);
    // addch(ch);
  }
  }

  checkPointerScroll(pointer, scroll);

  resetScreen(file, pointer, scroll, type);
}

#endif