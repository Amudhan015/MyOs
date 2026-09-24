#include <kernel/idt.h>
#include <stdint.h>

struct registers {
  uint32_t ds;
  uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
  uint32_t int_no, err_code;
  uint32_t eip, cs, eflags, useresp, ss;
};

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

extern void
terminal_writestring(const char *str); // stub for now, see note below

void isr_handler(struct registers regs) {
  volatile uint16_t *vga = (uint16_t *)0xB8000;
  const char *msg = exception_messages[regs.int_no];

  for (int i = 0; msg[i] != '\0'; i++) {
    vga[80 + i] = (uint16_t)msg[i] | (0x4F << 8); // white on red, row 2
  }

  for (;;) {
    __asm__("hlt");
  }
}