#include <kernel/gdt.h>
#include <kernel/idt.h>

void kernel_main(void) {
  gdt_install();
  idt_install();

  volatile uint16_t *vga = (uint16_t *)0xB8000;
  const uint16_t blank = ((uint16_t)' ') | (0x0F << 8);

  for (int i = 0; i < 80 * 25; i++) {
    vga[i] = blank;
  }

  const char *msg = "Hello, kernel!";
  for (int i = 0; msg[i] != '\0'; i++) {
    vga[i] = (uint16_t)msg[i] | (0x0F << 8);
  }
}
