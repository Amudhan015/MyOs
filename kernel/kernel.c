#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/pic.h>
#include <kernel/serial.h>
#include <kernel/terminal.h>

void kernel_main(void) {
  gdt_install();
  serial_initialize();
  idt_install();
  pic_remap();

  terminal_initialize();
  terminal_set_cursor_shape(0, 15); // full-height block
  terminal_writestring("Hello, kernel!\n");

  pic_unmask_irq(0);
  pic_unmask_irq(1);
  __asm__ volatile("sti");
}
