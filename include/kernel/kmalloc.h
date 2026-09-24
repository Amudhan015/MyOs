#ifndef KMALLOC_H
#define KMALLOC_H

#include <kernel/int.h>

void kmalloc_init(void);
void *kmalloc(u32 size);
void kfree(void *ptr);

#endif
