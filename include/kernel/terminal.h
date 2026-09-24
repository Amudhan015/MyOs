#ifndef TERMINAL_H
#define TERMINAL_H

#include <kernel/int.h>

void terminal_initialize(void);
void terminal_putchar(char c);
void terminal_writestring(const char *str);
void terminal_scroll(int delta);
void terminal_set_cursor_shape(u8 start_scanline, u8 end_scanline);

#endif
