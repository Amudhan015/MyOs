#ifndef SERIAL_H
#define SERIAL_H

#include <kernel/int.h>

void serial_initialize(void);
void serial_putchar(char c);
void serial_writestring(const char *str);
void serial_write_hex_byte(u8 val);

#endif
