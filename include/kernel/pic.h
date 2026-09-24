#ifndef PIC_H
#define PIC_H

#include <kernel/int.h>

void pic_remap(void);
void pic_unmask_irq(u8 irq_line);

u8 debug_read_pic1_mask(void);

#endif
