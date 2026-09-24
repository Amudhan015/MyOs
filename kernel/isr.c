#include <kernel/int.h>
#include <kernel/isr.h>

static const char *exception_messages[] = {"Division By Zero",
                                           "Debug",
                                           "Non Maskable Interrupt",
                                           "Breakpoint",
                                           "Into Detected Overflow",
                                           "Out of Bounds",
                                           "Invalid Opcode",
                                           "No Coprocessor",
                                           "Double Fault",
                                           "Coprocessor Segment Overrun",
                                           "Bad TSS",
                                           "Segment Not Present",
                                           "Stack Fault",
                                           "General Protection Fault",
                                           "Page Fault",
                                           "Unknown Interrupt",
                                           "Coprocessor Fault",
                                           "Alignment Check",
                                           "Machine Check",
                                           "Reserved",
                                           "Reserved",
                                           "Reserved",
                                           "Reserved",
                                           "Reserved",
                                           "Reserved",
                                           "Reserved",
                                           "Reserved",
                                           "Reserved",
                                           "Reserved",
                                           "Reserved",
                                           "Reserved",
                                           "Reserved"};

void isr_handler(struct registers regs) {
  volatile u16 *vga = (u16 *)0xB8000;
  const char *msg = exception_messages[regs.int_no];

  for (int i = 0; msg[i] != '\0'; i++) {
    vga[80 + i] = (u16)msg[i] | (0x4F << 8);
  }

  for (;;) {
    __asm__("hlt");
  }
}
