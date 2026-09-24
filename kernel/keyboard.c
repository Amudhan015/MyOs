#include <kernel/int.h>
#include <kernel/keyboard.h>

#define KEYBOARD_DATA_PORT 0x60

static inline u8 inb(u16 port) {
  u8 ret;
  __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
  return ret;
}

static volatile u16 *vga = (u16 *)0xB8000;
static int term_col = 0;
static int term_row = 1;

static const char scancode_ascii[128] = {
    0,   27,  '1',  '2',  '3',  '4', '5', '6',  '7', '8', '9', '0',
    '-', '=', '\b', '\t', 'q',  'w', 'e', 'r',  't', 'y', 'u', 'i',
    'o', 'p', '[',  ']',  '\n', 0,   'a', 's',  'd', 'f', 'g', 'h',
    'j', 'k', 'l',  ';',  '\'', '`', 0,   '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm',  ',',  '.',  '/', 0,   '*',  0,   ' '
    // remaining entries default to 0 — unmapped keys (F-keys, arrows, etc.)
};

static void kb_putchar(char c) {
  if (c == '\n') {
    term_col = 0;
    term_row++;
    return;
  }
  if (c == '\b') {
    if (term_col > 0)
      term_col--;
    vga[term_row * 80 + term_col] = ((u16)' ') | (0x0F << 8);
    return;
  }
  vga[term_row * 80 + term_col] = ((u16)c) | (0x0F << 8);
  term_col++;
}

void keyboard_handler(void) {
  u8 scancode = inb(KEYBOARD_DATA_PORT);

  if (scancode & 0x80) {
    return; // key release — ignore for now
  }

  char c = scancode_ascii[scancode];
  if (c) {
    kb_putchar(c);
  }
}
