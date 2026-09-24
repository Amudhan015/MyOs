#include <kernel/int.h>
#include <kernel/pit.h>

#define PIT_CHANNEL0_DATA 0x40
#define PIT_COMMAND 0x43
#define PIT_BASE_FREQUENCY 1193182

static inline void outb(u16 port, u8 val) {
  __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static volatile u32 tick_count = 0;

void pit_init(u32 frequency_hz) {
  u32 divisor = PIT_BASE_FREQUENCY / frequency_hz;

  outb(PIT_COMMAND,
       0x36); // channel 0, lobyte/hibyte access, mode 3 (square wave)

  outb(PIT_CHANNEL0_DATA, (u8)(divisor & 0xFF));
  outb(PIT_CHANNEL0_DATA, (u8)((divisor >> 8) & 0xFF));
}

void pit_tick(void) { tick_count++; }

u32 pit_get_ticks(void) { return tick_count; }
