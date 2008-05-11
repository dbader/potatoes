# etiOS makefile
# based on James M's kernel tutorials

SOURCES=boot/boot.o lib/string.o io/monitor.o init/interrupt.o init/idt.o init/isr.o init/main.o

CFLAGS=-nostdlib -nostdinc -fno-builtin -fno-stack-protector
LDFLAGS=-Tlink.ld
ASFLAGS=-felf

all: $(SOURCES) link

clean:
	-rm *.o kernel

link:
	ld $(LDFLAGS) -o kernel $(SOURCES)

.s.o:
	nasm $(ASFLAGS) $<
