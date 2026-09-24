#ifndef PAGING_H
#define PAGING_H

#include <kernel/int.h>

#define PAGE_PRESENT 0x1
#define PAGE_WRITABLE 0x2
#define PAGE_USER 0x4

typedef u32 page_table_entry_t;
typedef u32 page_directory_entry_t;

void paging_init(void);
void paging_map_page(u32 virt_addr, u32 phys_addr, u32 flags);

#endif
