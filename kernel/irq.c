#include <kernel/int.h>
#include <kernel/isr.h>
#include <kernel/keyboard.h>
#include <kernel/pit.h>

#define PIC1_COMMAND 0x20
#define PIC2_COMMAND 0xA0
#define PIC_EOI 0x20

static inline void outb(u16 port, u8 val) {
  __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

void irq_handler(struct registers regs) {
  if (regs.int_no == 32) { // IRQ0, post-remap = PIT timer
    pit_tick();
  }

  if (regs.int_no == 33) { // IRQ1 = keyboard
    keyboard_handler();
  }

  if (regs.int_no >= 40) {
    outb(PIC2_COMMAND, PIC_EOI);
  }
  outb(PIC1_COMMAND, PIC_EOI);
}
