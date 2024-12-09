
#include "scroll.h"

void checkPointerScroll(Cursor *pointer, TextScroll *scroll)
{
  if (pointer->line_count < scroll->start_line)
  {
    scroll->start_line = pointer->line_count;
  }
  else if (pointer->line_count >= (scroll->start_line + scroll->max_lines))
  {
    scroll->start_line = pointer->line_count - scroll->max_lines + 1;
  }

  if (pointer->character_count < scroll->start_col || pointer->character_count >= (scroll->start_col + scroll->max_width))
    scroll->start_col = pointer->character_count - scroll->max_width + 1;
}