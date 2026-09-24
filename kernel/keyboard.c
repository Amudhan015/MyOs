#include <kernel/int.h>
#include <kernel/keyboard.h>
#include <kernel/serial.h>
#include <kernel/terminal.h>

#define KEYBOARD_DATA_PORT 0x60
#define SCANCODE_EXTENDED_PREFIX 0xE0
#define SCANCODE_ARROW_UP 0x48
#define SCANCODE_ARROW_DOWN 0x50

static inline u8 inb(u16 port) {
  u8 ret;
  __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
  return ret;
}

static const char scancode_ascii[128] = {
    0,   27,  '1',  '2',  '3',  '4', '5', '6',  '7', '8', '9', '0',
    '-', '=', '\b', '\t', 'q',  'w', 'e', 'r',  't', 'y', 'u', 'i',
    'o', 'p', '[',  ']',  '\n', 0,   'a', 's',  'd', 'f', 'g', 'h',
    'j', 'k', 'l',  ';',  '\'', '`', 0,   '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm',  ',',  '.',  '/', 0,   '*',  0,   ' '};

static int expecting_extended = 0;

void keyboard_handler(void) {
  u8 scancode = inb(KEYBOARD_DATA_PORT);

  if (scancode == SCANCODE_EXTENDED_PREFIX) {
    expecting_extended = 1;
    return;
  }

  if (expecting_extended) {
    expecting_extended = 0;
    if (scancode & 0x80)
      return;

    switch (scancode) {
    case SCANCODE_ARROW_UP:
      terminal_scroll(1);
      break;
    case SCANCODE_ARROW_DOWN:
      terminal_scroll(-1);
      break;
    default:
      break;
    }

    return;
  }

  if (scancode & 0x80)
    return;
  if (scancode >= 128)
    return;

  char c = scancode_ascii[scancode];
  if (c)
    terminal_putchar(c);
}
