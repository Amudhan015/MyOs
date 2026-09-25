#include <kernel/int.h>
#include <kernel/pmm.h>
#include <kernel/serial.h>

#define FRAME_SIZE 4096
#define MAX_FRAMES (1024 * 1024) // covers up to 4GB of address space

static u32 bitmap[MAX_FRAMES / 32]; // one bit per frame, packed into u32 words

extern u32 end; // linker-provided symbol — see linker.ld note below

static void set_frame(u32 frame_idx) {
  bitmap[frame_idx / 32] |= (1 << (frame_idx % 32));
}

static void clear_frame(u32 frame_idx) {
  bitmap[frame_idx / 32] &= ~(1 << (frame_idx % 32));
}

static int test_frame(u32 frame_idx) {
  return bitmap[frame_idx / 32] & (1 << (frame_idx % 32));
}

void pmm_init(struct multiboot_info *mb_info) {
  // Start pessimistic: assume everything is used/reserved...
  for (u32 i = 0; i < MAX_FRAMES / 32; i++) {
    bitmap[i] = 0xFFFFFFFF;
  }

  // ...then mark only the regions GRUB reports as truly available.
  if (mb_info->flags & (1 << 6)) {
    u32 entry_addr = mb_info->mmap_addr;
    u32 mmap_end = mb_info->mmap_addr + mb_info->mmap_length;

    while (entry_addr < mmap_end) {
      struct multiboot_mmap_entry *entry =
          (struct multiboot_mmap_entry *)entry_addr;

      if (entry->type == 1) { // available RAM
        u32 start_frame = (u32)(entry->addr / FRAME_SIZE);
        u32 frame_count = (u32)(entry->len / FRAME_SIZE);
        for (u32 f = 0; f < frame_count; f++) {
          if (start_frame + f < MAX_FRAMES) {
            clear_frame(start_frame + f);
          }
        }
      }

      entry_addr += entry->size + sizeof(entry->size);
    }
  }

  // Now protect the kernel's own footprint — it's "available RAM" as far
  // as GRUB is concerned, but it's occupied by code/data that's running
  // right now, so it must not be handed out.
  u32 kernel_end_frame = ((u32)&end) / FRAME_SIZE + 1;
  for (u32 f = 0; f < kernel_end_frame; f++) {
    set_frame(f);
  }

  serial_writestring("PMM initialized\n");
}

u32 pmm_alloc_frame(void) {
  for (u32 i = 0; i < MAX_FRAMES; i++) {
    if (!test_frame(i)) {
      set_frame(i);
      return i * FRAME_SIZE;
    }
  }
  return 0; // out of memory — 0 is used as a sentinel; frame 0 is always
            // reserved anyway
}

void pmm_free_frame(u32 frame_addr) {
  u32 idx = frame_addr / FRAME_SIZE;
  if (idx >= MAX_FRAMES) {
    serial_writestring("pmm_free_frame: address out of range\n");
    return;
  }
  if (!test_frame(idx)) {
    serial_writestring("pmm_free_frame: double-free detected\n");
    return;
  }
  clear_frame(idx);
}
