/* $Id: init.h 266 2009-10-14 08:15:36Z dtraytel $
********************************************************************************
* _____   ____ _______    _______ ____  ______  _____                          *
*|  __ \ / __ \__   __|/\|__   __/ __ \|  ____|/ ____|          Copyright 2008 *
*| |__) | |  | | | |  /  \  | | | |  | | |__  | (___              Daniel Bader *
*|  ___/| |  | | | | / /\ \ | | | |  | |  __|  \___ \           Vincenz Doelle *
*| |    | |__| | | |/ ____ \| | | |__| | |____ ____) |    Johannes Schamburger *
*|_|     \____/  |_/_/    \_\_|  \____/|______|_____/          Dmitriy Traytel *
*                                                                              *
*      Practical Oriented TeAching Tool, Operating (and) Educating System      *
*                                                                              *
*                           www.potatoes-project.tk                            *
*******************************************************************************/

/**
 * @file
 * Basic difinitions for functions used in the main()-function of the kernel
 *
 * @author Dmitriy Traytel
 * @author $LastChangedBy: dtraytel $
 * @version $Rev: 266 $
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
