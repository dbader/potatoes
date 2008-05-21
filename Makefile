#
# etiOS makefile
#
# You must somehow ensure that boot.o is the first file passed to the linker.
# Right now this happens automagically through the order of PROJDIRS.
#
# To enable another subsystem for compilation add its subdirectory to PROJDIRS and
# let "make" handle the rest.
#
# Be sure to update LOOPDEV and LOOPMNT if necessary.
#
# Inspired by http://www.osdev.org/wiki/Makefile.
#

PROJDIRS = src/kernel/boot src/kernel/include src/kernel/init src/kernel/io src/kernel/lib \
	   src/kernel/mm src/kernel/pm

# Adds all source files in the subdirs referenced by PROJDIRS
SRCFILES := $(shell find $(PROJDIRS) -mindepth 1 -maxdepth 3 -name "*.c")
HDRFILES := $(shell find $(PROJDIRS) -mindepth 1 -maxdepth 3 -name "*.h")
ASMFILES := $(shell find $(PROJDIRS) -mindepth 1 -maxdepth 3 -name "*.s")
OBJFILES := $(patsubst %.s,%.o,$(ASMFILES)) $(patsubst %.c,%.o,$(SRCFILES))
DEPFILES := $(patsubst %.c,%.d,$(SRCFILES))
ALLFILES := $(SRCFILES) $(HDRFILES) $(ASMFILES) 

# Generated files that should be deleted by make clean
GENFILES := src/kernel/kernel src/kernel/kernel.map floppy.img etios.pdf

# Toolflags
CFLAGS=-DNDEBUG -nostdlib -nostdinc -fno-builtin -fno-stack-protector -std=c99
LDFLAGS=-m elf_i386 -Tsrc/kernel/link.ld
ASFLAGS=-felf

# The loopback device for the image
LOOPDEV=/dev/loop0

# The mount point for the loopback image
LOOPMNT=/mnt2

.PHONY: all clean fiximg runbochs doc todo image link

all: kernel image doc

help:
	@echo "Available make targets:"
	@echo
	@echo "all	- builds the kernel, floppy image and documentation"
	@echo "clean	- removes all generated files"
	@echo "doc	- builds doxygen documentation"
	@echo "fiximg	- unmounts the image and disables loopback"
	@echo "image	- builds floppy image (floppy.img)"
	@echo "kernel	- builds the kernel"
	@echo "runbochs	- starts bochs"
	@echo "todo	- search sourcecode for TODOs"
	@echo "help	- displays this list"

clean:
	@echo " CLEAN"
	-@for file in $(OBJFILES) $(DEPFILES) $(GENFILES); do if [ -f $$file ]; then rm $$file; fi; done
	-@for dir in doc/html doc/latex; do if [ -d $$dir ]; then rm -r $$dir; fi; done
	
runbochs: image
	-@sudo /sbin/losetup $(LOOPDEV) floppy.img
	-@sudo bochs -f src/tools/bochsrc > /dev/null
	@sudo /sbin/losetup -d $(LOOPDEV)
	
doc:
	@echo " DOXYGEN"
	@doxygen > /dev/null
	@cd doc/latex && $(MAKE) > /dev/null 2> /dev/null
	@cp doc/latex/refman.pdf ./etios.pdf
	
# Sometimes the image remains mounted after an error, use this target to fix this.	
fiximg:
	@echo "Unmounting image..."
	-@sudo umount $(LOOPDEV)
	@echo "Disabling loopback..."
	-@sudo /sbin/losetup -d $(LOOPDEV)
	@echo "Done."
	
todo:
	@echo "TODO:"
	-@for file in $(ALLFILES); do grep -H TODO $$file; done; true
	
image: kernel
	@echo " IMAGE  floppy.img"
	@dd if=/dev/zero of=floppy.img bs=512 count=1440 status=noxfer 2> /dev/null
	@sudo /sbin/losetup $(LOOPDEV) floppy.img
	@sudo mkfs -t ext2 $(LOOPDEV) > /dev/null 2> /dev/null

	@sudo mount -t ext2 $(LOOPDEV) $(LOOPMNT)
	@sudo mkdir $(LOOPMNT)/grub
	@sudo cp -r image/* $(LOOPMNT)
	@sudo cp src/kernel/kernel $(LOOPMNT)
	@sudo umount $(LOOPDEV)
	
	@echo "(fd0) $(LOOPDEV)" > grubdevice.map
	@echo "root (fd0)" > grubconf.conf
	@echo "setup (fd0)" >> grubconf.conf
	@echo "quit" >> grubconf.conf
	@sudo cat grubconf.conf | sudo grub --batch --device-map=grubdevice.map $(LOOPDEV) > /dev/null 2> /dev/null
	@rm grubdevice.map grubconf.conf
	
	@sudo /sbin/losetup -d $(LOOPDEV)

kernel: $(OBJFILES) Makefile
	@echo " LD	src/kernel/kernel"
	@ld $(LDFLAGS) -Map src/kernel/kernel.map -o src/kernel/kernel $(OBJFILES)

-include $(DEPFILES)

%.o: %.s Makefile
	@echo " AS	$(patsubst functions/%,%,$@)"
	@nasm -felf -o $@ $<

%.o: %.c Makefile
	@echo " CC	$(patsubst functions/%,%,$@)"
	@$(CC) $(CFLAGS) -MMD -MP -MT "$*.d $*.t" -g -c $< -o $@
