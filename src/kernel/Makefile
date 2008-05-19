# etiOS makefile
# based on James M's kernel tutorials

SOURCES=boot/boot.o lib/string.o io/io_keyboard.o io/io_monitor.o io/io_timer.o \
        init/gdt.o init/global.o init/interrupt.o init/idt.o init/isr.o \
        init/irq.o init/tests.o init/main.o mm/malloc_free.o lib/stdio.o

CFLAGS=-nostdlib -nostdinc -fno-builtin -fno-stack-protector -std=c99
LDFLAGS=-Tlink.ld
ASFLAGS=-felf

all: $(SOURCES) link

clean:
	-rm $(SOURCES) kernel

link:
	ld $(LDFLAGS) -o kernel $(SOURCES)

.s.o:
	nasm $(ASFLAGS) $<
	