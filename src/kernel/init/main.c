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
        
        gdt_init();
        idt_init();
        isr_init();
        irq_init();
        timer_init(FREQUENCY);
        set_interrupts();
        
        //grubstruct_test(mboot_ptr);
        //strings_test();       
        //draw_test();
        //printf_test();
        //assert_test();
        //malloc_test();
        strsep_test();
        sleep_test();
        
        for(;;);
	return 0;
}
