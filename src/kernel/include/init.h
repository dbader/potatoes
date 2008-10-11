/* $Id$
      _   _  ____   _____
     | | (_)/ __ \ / ____|
  ___| |_ _| |  | | (___
 / _ \ __| | |  | |\___ \  Copyright 2008 Daniel Bader, Vincenz Doelle,
|  __/ |_| | |__| |____) |        Johannes Schamburger, Dmitriy Traytel
 \___|\__|_|\____/|_____/

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file
 * Basic difinitions for functions used in the main()-function of the kernel
 *
 * @author Dmitriy Traytel
 * @author $LastChangedBy$
 * @version $Rev$
 */

#ifndef __INIT_H
#define __INIT_H

#include "types.h"

/**
 * Multiboot structure.
 * @see http://www.gnu.org/software/grub/manual/multiboot/multiboot.html
 */
struct multiboot {
        uint32 flags;
        uint32 mem_lower;
        uint32 mem_upper;
        uint32 boot_device;
        uint32 cmdline;
        uint32 mods_count;
        uint32 mods_addr;
        uint32 num;
        uint32 size;
        uint32 addr;
        uint32 shndx;
        uint32 mmap_length;
        uint32 mmap_addr;
        uint32 drives_length;
        uint32 drives_addr;
        uint32 config_table;
        uint32 boot_loader_name;
        uint32 apm_table;
        uint32 vbe_control_info;
        uint32 vbe_mode_info;
        uint32 vbe_mode;
        uint32 vbe_interface_seg;
        uint32 vbe_interface_off;
        uint32 vbe_interface_len;
}  __attribute__((packed));

/** Global pointer to multiboot structure */
extern struct multiboot *g_mboot_ptr;

//init-functions
void mm_init(uint32 start, uint32 end);
void mm_init2();
void io_init();
void pm_init();
void fs_init();
void fs_shutdown();

void panic(char *msg);


//test-functions
void do_tests();

#endif /* init.h */
