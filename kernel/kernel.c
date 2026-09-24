#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/multiboot.h>
#include <kernel/pic.h>
#include <kernel/pmm.h>
#include <kernel/serial.h>
#include <kernel/terminal.h>

void kernel_main(struct multiboot_info *mb_info) {
  gdt_install();
  idt_install();
  pic_remap();
  serial_initialize();

  terminal_initialize();
  terminal_set_cursor_shape(0, 15); // full-height block
  terminal_writestring("Hello, kernel!\n");

  pmm_init(mb_info);

  pic_unmask_irq(0);
  pic_unmask_irq(1);
  __asm__ volatile("sti");
}
