#include <kernel/int.h>
#include <kernel/kmalloc.h>
#include <kernel/pmm.h>
#include <kernel/serial.h>
#include <kernel/task.h>

#define TASK_STACK_SIZE 4096

extern void switch_task(u32 *old_esp_ptr, u32 new_esp);

static task_t *current_task = 0;
static task_t *task_list_head = 0;

void tasking_init(void) {
  // The currently executing flow (kernel_main itself) becomes the first
  // task, so there's always something valid to switch *from*.
  task_t *main_task = (task_t *)kmalloc(sizeof(task_t));
  main_task->stack_base = 0;   // kernel_main's stack isn't ours to free
  main_task->next = main_task; // circular list of one
  current_task = main_task;
  task_list_head = main_task;
}

void task_create(void (*entry_point)(void)) {
  task_t *task = (task_t *)kmalloc(sizeof(task_t));
  u32 stack_phys = pmm_alloc_frame();

  if (stack_phys == 0) {
    serial_writestring("task_create: out of memory for stack\n");
    return;
  }

  u32 *stack = (u32 *)stack_phys;
  task->stack_base = stack;

  u32 *sp = (u32 *)((u8 *)stack + TASK_STACK_SIZE);
  *(--sp) = (u32)entry_point;
  *(--sp) = 0x200;
  *(--sp) = 0; // ebp
  *(--sp) = 0; // ebx
  *(--sp) = 0; // esi
  *(--sp) = 0; // edi

  task->esp = (u32)sp;

  task->next = current_task->next;
  current_task->next = task;
}

void schedule(void) {
  task_t *prev = current_task;
  current_task = current_task->next;

  if (prev != current_task) {
    switch_task(&prev->esp, current_task->esp);
  }
}
