
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <curses.h>
#include "textfile.h"
#include "cursor.h"
#include "scroll.h"
#include "currentFileEditor.h"

// Function to append a string to a dynamically growing string
void appendToString(char **str, size_t *length, size_t *capacity, const char *toAppend, size_t toAppendLength)
{
  // Resize if needed
  if (*length + toAppendLength + 1 > *capacity)
  {                                                 // +1 for the null terminator
    *capacity = (*length + toAppendLength + 1) * 2; // Double the size
    char *newStr = (char *)realloc(*str, sizeof(char) * (*capacity));
    if (!newStr)
    {
      perror("Failed to realloc memory");
      exit(EXIT_FAILURE);
    }
    *str = newStr;
  }

  // Append the new content
  memcpy(*str + *length, toAppend, toAppendLength);
  *length += toAppendLength;
  (*str)[*length] = '\0'; // Null-terminate
}

void sendMail(Textfile *mail, Textfile *subject, Textfile *recipent)
{
  char *command = (char *)malloc(sizeof(char) * DEFAULT_CHARS_PER_LINE);
  size_t length = 0;
  size_t capacity = DEFAULT_CHARS_PER_LINE;
  const char *txt = "echo 'Hello Karn,\n this is a test email!' | mail -s 'Test Email by term' karankavatra@gmail.com";

  appendToString(&command, &length, &capacity, "echo \'", strlen("echo \'"));

  for (size_t i = 0; i < mail->count; i++)
  {
    appendToString(&command, &length, &capacity, mail->lines[i]->data, mail->lines[i]->length);
  }
  appendToString(&command, &length, &capacity, "\' | mail -s \'", strlen("\' | mail -s \'"));

  for (size_t i = 0; i < subject->count; i++)
  {
    appendToString(&command, &length, &capacity, subject->lines[i]->data, subject->lines[i]->length);
  }
  appendToString(&command, &length, &capacity, "\' ", strlen("\' "));

  for (size_t i = 0; i < recipent->count; i++)
  {
    appendToString(&command, &length, &capacity, recipent->lines[i]->data, recipent->lines[i]->length);
    appendToString(&command, &length, &capacity, " ", strlen(" "));
  }
  system(command);
  // printf("%s", command);
  free(command);
}

int main()
{
  Textfile *mail = NULL;
  allocateTextfile(&mail);

  Textfile *subject = NULL;
  allocateTextfile(&subject);

  Textfile *recipient = NULL;
  allocateTextfile(&recipient);

  Cursor mailPointer;
  mailPointer.line_count = 0;
  mailPointer.character_count = 0;

  Cursor subjectPointer;
  subjectPointer.line_count = 0;
  subjectPointer.character_count = 0;

  Cursor recipientPointer;
  recipientPointer.line_count = 0;
  recipientPointer.character_count = 0;

  // initialise ncurses
  initscr();            // Start ncurses mode
  cbreak();             // Disable line buffering, allowing input immediately
  noecho();             // Do not display typed characters
  keypad(stdscr, TRUE); // Enable special key handling
  curs_set(1);          // Show cursor
  start_color();        // Enable color mode
  use_default_colors();

  init_pair(1, COLOR_BLACK, COLOR_WHITE); // Create color pair 1

  int rows, cols;
  getmaxyx(stdscr, rows, cols);
  // printf("Terminal Size: %d %d\r\n", rows, cols);

  TextScroll mailScroll;
  mailScroll.start_line = 0;
  mailScroll.start_col = 0;
  mailScroll.max_lines = rows - 2; // Leave one line for status or buffer
  mailScroll.max_width = cols;

  TextScroll subjectScroll;
  subjectScroll.start_line = 0;
  subjectScroll.start_col = 0;
  subjectScroll.max_lines = rows - 2; // Leave one line for status or buffer
  subjectScroll.max_width = cols;

  TextScroll recipientScroll;
  recipientScroll.start_line = 0;
  recipientScroll.start_col = 0;
  recipientScroll.max_lines = rows - 2; // Leave one line for status or buffer
  recipientScroll.max_width = cols;

  int index = 0;
  // printf("Columns: %d", cols);
  // makeCorrectLength("Message", &recipientScroll);

  attron(COLOR_PAIR(1));
  displayPageName("Message", &mailScroll);
  attroff(COLOR_PAIR(1));
  move(mailPointer.line_count - mailScroll.start_line, mailPointer.character_count - mailScroll.start_col);

  refresh();

  while (1)
  {
    int ch = getch();
    // printf("%d", ch);
    if (ch == CTRL('r'))
      break;

    if (ch == KEY_ESC)
    {
      index++;
      if (index == 3)
        index = 0;
      switch (index)
      {
      case 0:
        resetScreen(mail, &mailPointer, &mailScroll, "Message");
        break;
      case 1:
        resetScreen(subject, &subjectPointer, &subjectScroll, "Subject");
        break;
      case 2:
        resetScreen(recipient, &recipientPointer, &recipientScroll, "Recipients");
        break;
      }

      continue;
    }

    switch (index)
    {
    case 0:
      performOperation(ch, mail, &mailPointer, &mailScroll, "Message");
      break;
    case 1:
      performOperation(ch, subject, &subjectPointer, &subjectScroll, "Subject");
      break;
    case 2:
      performOperation(ch, recipient, &recipientPointer, &recipientScroll, "Recipients");
      break;
    }
  }

  endwin();
  // printf("%lu %lu\r\n", scroll.start_line, scroll.start_col);
  // printf("%lu %lu\r\n", scroll.max_lines, scroll.max_width);
  // printf("%lu %lu\r\n", pointer.line_count, pointer.character_count);
  printf("Message:\r\n");
  printTextfile(mail);
  printf("\r\nSubject:\r\n");
  printTextfile(subject);
  printf("\r\nRecipients:\r\n");
  printTextfile(recipient);
  sendMail(mail, subject, recipient);
  // const char *command = "echo 'Hello Karn,\n this is a test email!' | mail -s 'Test Email by term' karankavatra@gmail.com";
  // system(command);
  deallocateTextfile(mail);
  deallocateTextfile(subject);
  deallocateTextfile(recipient);
  return 0;
}