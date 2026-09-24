CC = i686-elf-gcc
AS = i686-elf-as
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -Iinclude
LDFLAGS = -T linker.ld -ffreestanding -O2 -nostdlib -lgcc

OBJS = boot/boot.o kernel/kernel.o kernel/gdt.o kernel/gdt_asm.o \
       kernel/idt.o kernel/idt_asm.o kernel/isr.o kernel/isr_asm.o \
	   kernel/pic.o

all: myos.bin

myos.bin: $(OBJS) linker.ld
	$(CC) $(LDFLAGS) -o $@ $(OBJS)
	grub-file --is-x86-multiboot $@ && echo "multiboot confirmed"

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(AS) $< -o $@

run: myos.bin
	qemu-system-i386 -kernel myos.bin

clean:
	rm -f $(OBJS) myos.bin

.PHONY: all run clean
