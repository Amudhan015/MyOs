#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/kmalloc.h>
#include <kernel/multiboot.h>
#include <kernel/paging.h>
#include <kernel/pic.h>
#include <kernel/pit.h>
#include <kernel/pmm.h>
#include <kernel/serial.h>
#include <kernel/terminal.h>

void kernel_main(struct multiboot_info *mb_info) {
  // --- Interrupt infrastructure: must all be in place before sti ---
  gdt_install();
  idt_install();
  pic_remap();

  // --- Basic output, needed for everything after this to be debuggable ---
  serial_initialize();
  terminal_initialize();
  terminal_writestring("Hello, kernel!\n");

  // --- Device-specific interrupt sources, configured before enabling them ---
  pit_init(100);

  // --- Enable interrupts as early as possible — everything below this
  //     line can now safely assume timer ticks and keypresses are live ---
  pic_unmask_irq(0); // timer
  pic_unmask_irq(1); // keyboard
  __asm__ volatile("sti");

  // --- Memory subsystem ---
  pmm_init(mb_info);
  paging_init();
  kmalloc_init();

  // Future subsystems (scheduler, etc.) go here, after interrupts are
  // already live and memory management is ready.
}
