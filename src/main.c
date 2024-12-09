
#include <stdio.h>
#include <fcntl.h>  // For open()
#include <stdlib.h> // For mkstemp()
#include <string.h> // For strlen()
#include <unistd.h> // For write()
#include <stdint.h>
#include <sys/types.h> // For pid_t
#include <sys/wait.h>  // For wait

#include <curses.h>

#include "currentFileEditor.h"

#define _POSIX_C_SOURCE 200809L

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
  char tmp_name[] = "mailTemp_XXXXXX";
  int fd = mkstemp(tmp_name); // Create a temporary file for the mail
  if (fd == -1)
  {
    perror("mkstemp failed");
    perror("Mail not Sent");
    return;
  }
  // Now, unlink the file after creation so now the file is invisible by filesystem and only accessable by file descriptor and will delete itself
  if (unlink(tmp_name) == -1)
  {
    perror("unlink failed");
    perror("Mail not Sent");
    close(fd);
    return;
  }

  char *mailString = malloc(sizeof(char) * DEFAULT_CHARS_PER_LINE);
  size_t length = 0;
  size_t capacity = DEFAULT_CHARS_PER_LINE;

  for (size_t i = 0; i < mail->count; i++)
  {
    appendToString(&mailString, &length, &capacity, mail->lines[i]->data, mail->lines[i]->length);
  }

  // Write data to the file using the file descriptor
  ssize_t bytes_written = write(fd, mailString, strlen(mailString));
  if (bytes_written == -1)
  {
    perror("write failed");
    perror("Mail not Sent");
    close(fd);
    free(mailString);
    return;
  }

  fsync(fd);              // Ensures all written data is flushed to disk.
  lseek(fd, 0, SEEK_SET); // Resets the file offset to the beginning for reading.

  char *subjectString = malloc(sizeof(char) * DEFAULT_CHARS_PER_LINE);
  length = 0;
  capacity = DEFAULT_CHARS_PER_LINE;
  for (size_t i = 0; i < subject->count; i++)
  {
    appendToString(&subjectString, &length, &capacity, subject->lines[i]->data, subject->lines[i]->length);
  }

  char *recipientString = malloc(sizeof(char) * DEFAULT_CHARS_PER_LINE);
  length = 0;
  capacity = DEFAULT_CHARS_PER_LINE;
  for (size_t i = 0; i < recipent->count; i++)
  {
    appendToString(&recipientString, &length, &capacity, recipent->lines[i]->data, recipent->lines[i]->length);
  }

  pid_t pid;
  if ((pid = fork()) == 0)
  {
    dup2(fd, 0); // make that file the new stdin (this duplicates the "|" part of the cmd line
    char *argv[] = {
        "/usr/bin/mail",
        "-s",
        subjectString,   // reduced to a single char array, not your 'Textfile'
        recipientString, // reduced to a single char array, not your 'Textfile'
        NULL};
    execv(argv[0], argv);
  }
  else if (pid > 0)
  {
    int status;
    wait(&status);
  }
  else
  {
    perror("Mail not Sent");
  }
  close(fd);

  // Free memory
  free(mailString);
  free(subjectString);
  free(recipientString);
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

  printf("Message:\r\n");
  printTextfile(mail);
  printf("\r\nSubject:\r\n");
  printTextfile(subject);
  printf("\r\nRecipients:\r\n");
  printTextfile(recipient);

  sendMail(mail, subject, recipient);

  deallocateTextfile(mail);
  deallocateTextfile(subject);
  deallocateTextfile(recipient);
  return 0;
}