#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <kernel/int.h>

struct multiboot_mmap_entry {
  u32 size;
  u64 addr;
  u64 len;
  u32 type; // 1 = available RAM, anything else = reserved/unusable
} __attribute__((packed));

struct multiboot_info {
  u32 flags;
  u32 mem_lower, mem_upper;
  u32 boot_device;
  u32 cmdline;
  u32 mods_count, mods_addr;
  u32 syms[4];
  u32 mmap_length;
  u32 mmap_addr;
  // remaining fields unused for now
} __attribute__((packed));

#endif
