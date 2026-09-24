#include <kernel/int.h>
#include <kernel/serial.h>

#define COM1 0x3F8

static inline void outb(u16 port, u8 val) {
  __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline u8 inb(u16 port) {
  u8 ret;
  __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
  return ret;
}

void serial_initialize(void) {
  outb(COM1 + 1, 0x00); // disable interrupts for this port
  outb(COM1 + 3, 0x80); // enable DLAB (to set baud rate divisor)
  outb(COM1 + 0, 0x03); // divisor low byte  — 38400 baud
  outb(COM1 + 1, 0x00); // divisor high byte
  outb(COM1 + 3, 0x03); // 8 bits, no parity, one stop bit
  outb(COM1 + 2, 0xC7); // enable FIFO, clear it, 14-byte threshold
  outb(COM1 + 4, 0x0B); // enable IRQs (unused for now), RTS/DSR set
}

static int transmit_empty(void) { return inb(COM1 + 5) & 0x20; }

void serial_putchar(char c) {
  while (!transmit_empty())
    ;
  outb(COM1, (u8)c);
}

void serial_writestring(const char *str) {
  for (int i = 0; str[i] != '\0'; i++)
    serial_putchar(str[i]);
}

void serial_write_hex_byte(u8 val) {
  const char hex[] = "0123456789ABCDEF";
  serial_putchar(hex[(val >> 4) & 0xF]);
  serial_putchar(hex[val & 0xF]);
  serial_putchar(' ');
}
