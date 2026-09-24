CC = i686-elf-gcc
AS = i686-elf-as
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Iinclude
LDFLAGS = -T linker.ld -ffreestanding -O2 -nostdlib -lgcc

SRCS_C = kernel/kernel.c kernel/gdt.c kernel/idt.c kernel/isr.c \
         kernel/pic.c kernel/irq.c kernel/keyboard.c kernel/terminal.c \
		 kernel/serial.c kernel/pmm.c kernel/paging.c kernel/kmalloc.c \
		 kernel/pit.c
SRCS_S = boot/boot.s kernel/gdt_asm.s kernel/idt_asm.s \
         kernel/isr_asm.s kernel/irq_asm.s

OBJS = $(patsubst %.c,build/%.o,$(SRCS_C)) \
       $(patsubst %.s,build/%.o,$(SRCS_S))

all: myos.bin

myos.bin: $(OBJS) linker.ld
	$(CC) $(LDFLAGS) -o $@ $(OBJS)
	grub-file --is-x86-multiboot $@ && echo "multiboot confirmed"

build/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

build/%.o: %.s
	@mkdir -p $(dir $@)
	$(AS) $< -o $@

run: myos.bin
	qemu-system-i386 -kernel myos.bin -serial stdio

clean:
	rm -rf build myos.bin

.PHONY: all run clean
