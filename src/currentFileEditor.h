#ifndef CURRENTFILEEDITOR_H
#define CURRENTFILEEDITOR_H

#include "textfile.h"
#include "cursor.h"
#include "scroll.h"

#define KEY_ALT 18
#define KEY_ESC 27
#ifndef CTRL
#define CTRL(c) ((c) & 037)
#endif

void displayPageName(const char *msg, TextScroll *scroll);

void redrawScreen(Textfile *file, TextScroll *scroll, const char *type);

void resetScreen(Textfile *file, Cursor *pointer, TextScroll *scroll, const char *type);

void performOperation(int ch, Textfile *file, Cursor *pointer, TextScroll *scroll, const char *type);

#endif