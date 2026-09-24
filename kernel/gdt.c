#include <kernel/gdt.h>

struct gdt_entry gdt[5];
struct gdt_ptr gp;

static void gdt_set_gate(int num, u32 base, u32 limit, u8 access, u8 gran) {
  gdt[num].base_low = (base & 0xFFFF);
  gdt[num].base_middle = (base >> 16) & 0xFF;
  gdt[num].base_high = (base >> 24) & 0xFF;

  gdt[num].limit_low = (limit & 0xFFFF);
  gdt[num].granularity = (limit >> 16) & 0x0F;

  gdt[num].granularity |= (gran & 0xF0);
  gdt[num].access = access;
}

extern void gdt_flush(u32);

void gdt_install(void) {
  gp.limit = (sizeof(struct gdt_entry) * 5) - 1;
  gp.base = (u32)&gdt;

  gdt_set_gate(0, 0, 0, 0, 0);                // null descriptor
  gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // kernel code
  gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // kernel data
  gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // user code
  gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // user data

  gdt_flush((u32)&gp);
}
