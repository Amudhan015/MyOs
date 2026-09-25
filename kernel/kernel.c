#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/kmalloc.h>
#include <kernel/multiboot.h>
#include <kernel/paging.h>
#include <kernel/pic.h>
#include <kernel/pit.h>
#include <kernel/pmm.h>
#include <kernel/serial.h>
#include <kernel/task.h>
#include <kernel/terminal.h>

void kernel_main(struct multiboot_info* mb_info) {
    gdt_install();
    idt_install();
    pic_remap();

    serial_initialize();
    terminal_initialize();
    terminal_writestring("Hello, kernel!\n");

    pit_init(100);

    pmm_init(mb_info);
    paging_init();
    kmalloc_init();

    tasking_init();

    // Only now is everything an interrupt handler might touch actually ready.
    pic_unmask_irq(0);
    pic_unmask_irq(1);
    __asm__ volatile("sti");

    while (1) {
        __asm__ volatile("hlt");
    }
}
