#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/pic.h>

void kernel_main(void) {
  gdt_install();
  idt_install();
  pic_remap();

  volatile u16 *vga = (u16 *)0xB8000;
  const u16 blank = ((u16)' ') | (0x0F << 8);

  for (int i = 0; i < 80 * 25; i++) {
    vga[i] = blank;
  }

  const char *msg = "Hello, kernel!";
  for (int i = 0; msg[i] != '\0'; i++) {
    vga[i] = (u16)msg[i] | (0x0F << 8);
  }

  pic_unmask_irq(1);
  pic_unmask_irq(0);

  __asm__ volatile("sti");
}
