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
	   src/kernel/mm src/kernel/pm src/kernel/fs src/apps

# Adds all source files in the subdirs referenced by PROJDIRS
SRCFILES := $(shell find $(PROJDIRS) -mindepth 1 -maxdepth 3 -name "*.c")
HDRFILES := $(shell find $(PROJDIRS) -mindepth 1 -maxdepth 3 -name "*.h")
ASMFILES := $(shell find $(PROJDIRS) -mindepth 1 -maxdepth 3 -name "*.s")
OBJFILES := $(patsubst %.s,%.o,$(ASMFILES)) $(patsubst %.c,%.o,$(SRCFILES))
DEPFILES := $(patsubst %.c,%.d,$(SRCFILES))
ALLFILES := $(SRCFILES) $(HDRFILES) $(ASMFILES) 

# Generated files that should be deleted by make clean
GENFILES := src/kernel/kernel src/kernel/kernel.map etios.pdf

# Toolflags
CFLAGS=-DNDEBUG -nostdlib -nostdinc -fno-builtin -fno-stack-protector -std=c99
LDFLAGS=-m elf_i386 -Tsrc/kernel/link.ld
ASFLAGS=-felf

# The size of hda (in MB)
HDASIZE=20

# The name of the OS in virtualbox
OSNAME=ETIOS

# The loopback device for the image
LOOPDEV=/dev/loop0

# The mount point for the loopback image
LOOPMNT=/mnt2

.PHONY: all bin2c clean fiximg runbochs doc todo fdimage hdimage link tools

all: kernel fdimage hdimage doc tools

help:
	@echo "Available make targets:"
	@echo
	@echo "all		- builds the kernel, floppy image and documentation"
	@echo "bin2c		- bin2c tool"
	@echo "clean		- removes all generated files"
	@echo "doc		- builds doxygen documentation"
	@echo "fiximg		- unmounts the image and disables loopback"
	@echo "fdimage		- builds floppy image (floppy.img)"
	@echo "hdimage		- builds hard disk image (hda.img)"
	@echo "kernel		- builds the kernel"
	@echo "mac_runbochs	- starts bochs (mac)"
	@echo "mac_image	- update floppy.img (mac)"
	@echo "runbochs  	- starts bochs"
	@echo "runvirtualbox	- starts virtualbox"
	@echo "runqemu		- starts qemu"
	@echo "todo		- search sourcecode for TODOs"
	@echo "tools		- all tools"
	@echo "usbstick   	- install etiOS on an usb stick"
	@echo "LOC		- show lines of codes and number of files"
	@echo "help		- displays this list"

clean:
	@echo " CLEAN"
	-@for file in $(OBJFILES) $(DEPFILES) $(GENFILES); do if [ -f $$file ]; then rm $$file; fi; done
	-@for dir in doc/html doc/latex; do if [ -d $$dir ]; then rm -r $$dir; fi; done
	
runbochs: fdimage hdimage
	@bochs -f src/tools/bochsrc

runvirtualbox: fdimage	
	@VBoxManage startvm $(OSNAME)
	
runqemu: fdimage
	@qemu -localtime -fda floppy.img -soundhw pcspk -hda hda.img #--full-screen
	
mac_runbochs: mac_image
	@/Applications/bochs.app/Contents/MacOS/bochs -q -f src/tools/bochsrc
	
doc: $(OBJFILES) Makefile
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
	
fdimage: kernel
	@echo " FDIMAGE floppy.img"
	@dd if=/dev/zero of=floppy.img bs=1024 count=1440 status=noxfer 2> /dev/null
	@sudo /sbin/losetup $(LOOPDEV) floppy.img
	@sudo mkfs -t vfat $(LOOPDEV) > /dev/null 2> /dev/null

	@sudo mount -t vfat $(LOOPDEV) $(LOOPMNT)
	@sudo mkdir $(LOOPMNT)/grub
	@-sudo cp -r image/* $(LOOPMNT)
	@sudo cp src/kernel/kernel $(LOOPMNT)
	@sudo umount $(LOOPDEV)
	
	@echo "(fd0) $(LOOPDEV)" > grubdevice.map
	@echo "root (fd0)" > grubconf.conf
	@echo "setup (fd0)" >> grubconf.conf
	@echo "quit" >> grubconf.conf
	@sudo cat grubconf.conf | sudo grub --batch --device-map=grubdevice.map $(LOOPDEV) > /dev/null 2> /dev/null
	@rm grubdevice.map grubconf.conf
	
	@sudo /sbin/losetup -d $(LOOPDEV)
	
hdimage:
	@echo " HDIMAGE hda.img"
	@rm -f hda.img
	@bximage -q -hd -mode=flat -size=$(HDASIZE) hda.img | grep ata0-master > temp
	@sed "s/ata0-master:.*/`cat temp | sed "s/  a/a/1"`/g" src/tools/bochsrc > temp
	@cat temp > src/tools/bochsrc
	@rm -f temp

# This only updates the kernel file and needs a bootable copy of floppy.img
mac_image: kernel
	@echo " FDIMAGE  floppy.img"
	@hdiutil attach floppy.img > /dev/null
	@cp src/kernel/kernel /Volumes/ETIOS_BOOT
	@hdiutil detach /Volumes/ETIOS_BOOT > /dev/null
	
bin2c: src/tools/bin2c/bin2c.c
	@echo "Building bin2c..."
	@echo " CC	$(patsubst functions/%,%,$@)"
	@gcc src/tools/bin2c/bin2c.c -o bin2c
	
tools: bin2c

# If the USB-Stick is for the EEEPC use:
#
# sudo make EXTRA_CFLAGS=-DEEEPC usbstick TARGET_DEV=... PART_SIZE=...
TARGET_DEV=undefined
PART_SIZE=undefined
usbstick: kernel
ifeq ($(TARGET_DEV),undefined)
	@echo "Error: Please specify a target device with TARGET_DEV=XXX"
	@false
endif
ifeq ($(PART_SIZE),undefined)
	@echo "Error: Please specify a partition size (in MB) with PART_SIZE=XXX"
	@false
endif
	@echo "Installing to $(TARGET_DEV), partition size is $(PART_SIZE) MB"
	@echo "\n*** THIS WILL ERASE ALL DATA ON $(TARGET_DEV) ***\n"
	@read -p "Do you want to proceed? [yes/no]: " REPLY ; test $$REPLY = "yes"
	@echo "Partitioning..."
	@sudo parted -s $(TARGET_DEV) rm 1
	@sudo parted -s $(TARGET_DEV) mkpart primary fat32 0 $(PART_SIZE)
	@echo "Formatting..."
	@sudo parted -s $(TARGET_DEV) mkfs 1 fat32
	@sudo parted -s $(TARGET_DEV) toggle 1 boot
	@sudo parted -s $(TARGET_DEV) print
	@echo "Waiting for automounter..."
	@sleep 5
	@# Unmount target device (sometimes the automounter mounts it)
	@if (test -n "`mount | grep $(TARGET_DEV)`") then sudo umount `mount | grep $(TARGET_DEV) | grep -o -e 'on \(/.*\) type' | sed 's/on //' | sed 's/ type//'`; fi
	@echo "Mounting $(TARGET_DEV)1 on $(LOOPMNT)..."
	@sudo mount $(TARGET_DEV)1 $(LOOPMNT)
	@echo "Copying grub and the kernel to $(LOOPMNT)..."
	@cp -r image/* $(LOOPMNT)
	@cp src/kernel/kernel $(LOOPMNT)
	@echo "Installing grub to $(TARGET_DEV)..."
	@echo "(hd0) $(TARGET_DEV)" > $(LOOPMNT)/boot/grub/device.map
	@sudo grub-install --root-directory=$(LOOPMNT) $(TARGET_DEV)
	@echo "Fixing up $(LOOPMNT)/boot/grub/menu.lst..."
	@echo "timeout 1" > $(LOOPMNT)/boot/grub/menu.lst
	@echo "title etiOS Kernel" >> $(LOOPMNT)/boot/grub/menu.lst
	@echo "root (hd0,0)" >> $(LOOPMNT)/boot/grub/menu.lst
	@echo "kernel /kernel" >> $(LOOPMNT)/boot/grub/menu.lst
	@echo "Unmounting..."
	@sudo umount $(LOOPMNT)
	@echo "Done. etiOS successfully installed to $(TARGET_DEV)."

LOC:
	@cat $(SRCFILES) $(HDRFILES) | sh commentfilter.sh > temp
	@sed '/^\s*\/\//d' temp > temp2
	@sed '/^\s*\x24/d' temp2 > temp
	@wc -l temp > temp2
	@echo "ETIOS now has \n\t`sed "s/\s.*//1" temp2` LOCs"
	@echo "\t`ls $(SRCFILES) | wc -w` source files"
	@echo "\t`ls $(HDRFILES) | wc -w` header files"
	@rm temp2
	@read -p "Do you want to take a look on the code? [yes/no]: " REPLY;\
	 case $$REPLY in\
  		 j*|J*|y*|Y*) vim temp; rm temp ;;\
  		 *) rm temp ;;\
  	 esac\

zip:
	@Xdialog --stdout --no-cancel --inputbox "Please enter your name:" 0 0 > ./temp
	@zip "`date +%Y-%m-%d-%H.%M.%S`-`cat temp`".zip $(SRCFILES) $(HDRFILES)
	@rm temp

kernel: $(OBJFILES) Makefile
	@echo " LD	src/kernel/kernel"
	@$(LD) $(LDFLAGS) -Map src/kernel/kernel.map -o src/kernel/kernel $(OBJFILES)

-include $(DEPFILES)

%.o: %.s Makefile
	@echo " AS	$(patsubst functions/%,%,$@)"
	@nasm -felf -o $@ $<

%.o: %.c Makefile
	@echo " CC	$(patsubst functions/%,%,$@)"
	@$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -m32 -MMD -MP -MT "$*.d $*.t" -g -c $< -o $@
