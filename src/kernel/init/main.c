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
 * Main kernel file. Includes the C code entry point.
 *
 * @author Daniel Bader
 * @author $LastChangedBy$
 * @version $Rev$
 */

#include "../include/init.h"
#include "../include/const.h"
#include "../include/stdio.h"
#include "../include/debug.h"
#include "../io/io.h"
#include "../pm/pm_main.h"
#include "../io/io_virtual.h"

/**
 * Global pointer to multiboot structure
 * As of now, this is only needed for mboot_test() in tests.c. This should later
 * be removed and only used in the kernel main(). 
 */
struct multiboot *g_mboot_ptr;

/** 
 * The address of end is equal to the end of kernel code in memory + 1. 
 * This constant gets defined in the linker script link.ld.
 * Example: mem_start = (uint32) &end; 
 */
extern int end;

/**
 * The address of start is equal to the start address of the kernel code in memory
 */
extern int start;

/**
 * Kernel panic function. Displays an error message and enters an infinite
 * loop thus effectively halting the kernel. This should only be called when
 * a non-recoverable ("fatal") error occurs inside the kernel.
 * 
 * @param msg Message to display.
 */
void panic(char *msg)
{
        clear_interrupts();
        
        // This didnt work for me... (daniel)
        //virt_monitor_cputs(get_active_virt_monitor(), "KERNEL PANIC: ", BLACK, RED);
        //virt_monitor_cputs(get_active_virt_monitor(), msg, BLACK, RED);
        //update_virt_monitor();
        
        monitor_cputs("KERNEL PANIC: ", BLACK, RED);
        monitor_cputs(msg, BLACK, RED);
        
        for (;;) 
                halt();
}

/**
 * C kernel entry point.
 * 
 * @param mboot_ptr The multiboot struct passed by the bootloader (grub). 
 */
extern void shell_main();
int main(struct multiboot *mboot_ptr)
{
        g_mboot_ptr = mboot_ptr;
         
        //mm_init((uint32)&end, 0x100000 + mboot_ptr->mem_upper * 1024);
        mm_init(0x300001, 0x100000 + mboot_ptr->mem_upper * 1024);
        io_init();      
        fs_init();
        pm_init();
        
        dprint_separator();
        printf("%{main:} %d bytes kernel stack\n", RED, 0x300000 - (uint32)&end);
        printf("%{main:} init complete at %d ticks.\n", RED, get_ticks());
        
        do_tests();
        //fs_shutdown();
        
        printf("%{main:} entering idle loop\n", RED);
        
        // launch the shell.
        pm_create_thread("SHELL", shell_main, 4096);
        
        // kernel idle loop              
        for (;;)
                halt();
        
        return 0;
}
