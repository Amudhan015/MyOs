#ifndef TASK_H
#define TASK_H

#include <kernel/int.h>

typedef struct task {
  u32 esp; // saved stack pointer — everything else is recovered from the stack
           // itself
  u32 *stack_base;   // for freeing later
  struct task *next; // circular linked list of all tasks
} task_t;

void tasking_init(void);
void task_create(void (*entry_point)(void));
void schedule(void);

#endif
