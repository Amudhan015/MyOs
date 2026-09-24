#ifndef IDT_H
#define IDT_H

#include <kernel/int.h>

struct idt_entry {
  u16 base_low;
  u16 selector;
  u8 zero;
  u8 flags;
  u16 base_high;
} __attribute__((packed));

struct idt_ptr {
  u16 limit;
  u32 base;
} __attribute__((packed));

void idt_install(void);

#endif
