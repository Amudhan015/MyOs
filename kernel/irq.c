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
  // EOI first, unconditionally — pit_tick() below may switch tasks and
  // never return to this exact point until this task's turn comes again,
  // so anything after a possible switch is unsafe to rely on.
  if (regs.int_no >= 40) {
    outb(PIC2_COMMAND, PIC_EOI);
  }
  outb(PIC1_COMMAND, PIC_EOI);

  if (regs.int_no == 32) {
    pit_tick();
  }
  if (regs.int_no == 33) {
    keyboard_handler();
  }
}
