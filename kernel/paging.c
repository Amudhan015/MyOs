#include <kernel/int.h>
#include <kernel/paging.h>
#include <kernel/pmm.h>
#include <kernel/serial.h>

#define ENTRIES_PER_TABLE 1024

__attribute__((aligned(
    4096))) static page_directory_entry_t page_directory[ENTRIES_PER_TABLE];

static inline void load_page_directory(u32 phys_addr) {
  __asm__ volatile("mov %0, %%cr3" : : "r"(phys_addr));
}

static inline void enable_paging(void) {
  u32 cr0;
  __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
  cr0 |= 0x80000000; // set bit 31 — the PG (paging enable) bit
  __asm__ volatile("mov %0, %%cr0" : : "r"(cr0));
}

void paging_map_page(u32 virt_addr, u32 phys_addr, u32 flags) {
  u32 pd_index = virt_addr >> 22;           // top 10 bits
  u32 pt_index = (virt_addr >> 12) & 0x3FF; // middle 10 bits

  if (!(page_directory[pd_index] & PAGE_PRESENT)) {
    u32 new_table_phys = pmm_alloc_frame();
    page_table_entry_t *new_table = (page_table_entry_t *)new_table_phys;

    for (int i = 0; i < ENTRIES_PER_TABLE; i++) {
      new_table[i] = 0;
    }

    page_directory[pd_index] =
        new_table_phys | PAGE_PRESENT | PAGE_WRITABLE | (flags & PAGE_USER);
  }

  page_table_entry_t *table =
      (page_table_entry_t *)(page_directory[pd_index] & ~0xFFF);
  table[pt_index] = (phys_addr & ~0xFFF) | flags | PAGE_PRESENT;
}

void paging_init(void) {
  for (int i = 0; i < ENTRIES_PER_TABLE; i++) {
    page_directory[i] = 0;
  }

  // Identity-map the first 4MB: virtual address X maps to physical
  // address X, for every 4KB page in this range. This keeps your
  // currently-running kernel code, VGA memory, and everything else
  // you've built so far at the exact same addresses they already use —
  // paging turns on underneath you without anything moving.
  for (u32 addr = 0; addr < 0x400000; addr += 4096) {
    paging_map_page(addr, addr, PAGE_PRESENT | PAGE_WRITABLE);
  }

  load_page_directory((u32)page_directory);
  enable_paging();

  serial_writestring("Paging enabled\n");
}
