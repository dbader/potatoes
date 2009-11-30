HOSTCC = gcc
HOSTCCFLAGS = -Wall

FILES = ./fs_block_dev.o \
        ./fs_bmap.o \
        ./fs_buf.o \
        ./fs_create_delete.o \
        ./fs_dir.o \
        ./fs_file_table.o \
        ./fs_inode_table.o \
        ./fs_main.o \
        ./fs_open_close.o \
        ./fs_read_write.o \
        ./fs_super.o \
        ./fs_tests.o

all: chips chipsfs

./%.o: ../../kernel/fs/%.c Makefile fs_wrappers.h
	@echo " HOSTCC	$@"
	@$(HOSTCC) $(HOSTCCFLAGS) -include fs_wrappers.h -nostdinc -nostdlib -fno-builtin --std=c99 -c $< -o $@

fs_wrappers.o: fs_wrappers.c chips.h Makefile
	@echo " HOSTCC	$@"
	@$(HOSTCC) $(HOSTCCFLAGS) fs_wrappers.c -c -o fs_wrappers.o

chips.o: chips.c chips.h Makefile
	@echo " HOSTCC	$@"
	@$(HOSTCC) $(HOSTCCFLAGS) chips.c -c -o $@

chips: $(FILES) fs_wrappers.o chips.o Makefile
	@echo " HOSTCC	$@"
	@$(HOSTCC) $(HOSTCCFLAGS) $(FILES) fs_wrappers.o chips.o -o $@

chipsfs.o: chipsfs.c chips.h Makefile
	@echo " HOSTCC	$@"
	@$(HOSTCC) $(HOSTCCFLAGS) -D_FILE_OFFSET_BITS=64 chipsfs.c -c -o $@

chipsfs: $(FILES) fs_wrappers.o chipsfs.o Makefile
	@echo " HOSTCC	$@"
	@$(HOSTCC) $(HOSTCCFLAGS) $(FILES) fs_wrappers.o chipsfs.o -lfuse -o $@

../../../%: %
	@echo " CP	$<"
	@cp $< $@

clean:
	rm -f *.o
