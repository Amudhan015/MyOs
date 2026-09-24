#ifndef PMM_H
#define PMM_H

#include <kernel/int.h>
#include <kernel/multiboot.h>

void pmm_init(struct multiboot_info *mb_info);
u32 pmm_alloc_frame(void);
void pmm_free_frame(u32 frame_addr);

#endif
