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
#include "../pm/pm_main.h"

/**
 * Global pointer to multiboot structure
 * As of now, this is only needed for mboot_test() in tests.c. This should later
 * be removed and only used in the kernel main(). 
 */
struct multiboot *g_mboot_ptr;

/** 
 * The address of end is equal to the end of kernel code in memory + 1. 
 * This constant gets defined in the linker script link.ld. 
 */
extern int end;

/**
 * Kernel panic function. Displays an error message and enters an infinite
 * loop thus effectively halting the kernel. This should only be called when
 * a non-recoverable ("fatal") error occurs inside the kernel.
 * 
 * @param msg Message to display.
 */
void panic(char *msg)
{
        puts_col("KERNEL PANIC: ", BLACK, RED);
        puts_col(msg, BLACK, RED);
        for (;;) ;
        //TODO panic() should disable interrupts.
}

/**
 * C kernel entry point.
 * 
 * @param mboot_ptr The multiboot struct passed by the bootloader (grub). 
 */
int main(struct multiboot *mboot_ptr)
{
        puts("etiOS - $Rev$ - "__DATE__" "__TIME__"\n\n");

        /* Some memory info. Most of this is of special importance to Johannes / MM. */
        g_mboot_ptr = mboot_ptr;
        printf("%d bytes lower memory starting at addr 0x0\n", g_mboot_ptr->mem_lower * 1024);
        printf("%d bytes upper memory starting at addr 0x%x\n", g_mboot_ptr->mem_upper * 1024, 1024 * 1024);
        printf("kernel ends at addr 0x%x\n", &end - 1);
        printf("memory begins at addr 0x%x\n\n", &end);

        mm_init(0x009FFFFF, 0x01FFFFFF);
        //TODO: replace with call to io_init().
        idt_init();
        isr_init();
        irq_init();
        timer_init(FREQUENCY);
        set_interrupts();
        
        pm_init();
        
        //TODO: call fs_init();
        
        do_tests();
        
        for(;;);
	return 0;
}
