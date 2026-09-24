#ifndef PIT_H
#define PIT_H

#include <kernel/int.h>

void pit_init(u32 frequency_hz);
u32 pit_get_ticks(void);

void pit_tick(void);

#endif
