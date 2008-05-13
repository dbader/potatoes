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
#include "../include/assert.h"
#include "../include/string.h"

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
}

#define MBOOT_INFO(x) puts("\n"); puts(#x); puts(": "); puti(mboot_ptr->x);

/**
 * C kernel entry point.
 * 
 * @param mboot_ptr The multiboot struct passed by the bootloader (grub). 
 */
int main(struct multiboot *mboot_ptr)
{
        puts("etiOS - $Rev$ - "__DATE__" "__TIME__"\n\n");
        
        MBOOT_INFO(mem_lower)
        MBOOT_INFO(mem_upper)
        MBOOT_INFO(size)
        MBOOT_INFO(flags)
        MBOOT_INFO(addr)
        puts("\n\n");
        
        int i=0;
        idt_init();
        isr_init();    
        
        /* ------------------------- */
        /* test some string stuff */
        char f[100];
        char t[] = "This BLABLA";
        char t2[] = "is a STRING.H";
        char t3[] = " test.";
        bzero(f, sizeof(f));
        strncat(f, t, 5);
        strcat(f, t2);
        strcat(f, t3);
        puts(f); puts("\n\n");
        /* ------------------------- */
        
        //drawtest();
        
        // test ASSERT() macro
        ASSERT(TRUE)
        ASSERT(FALSE)
        ASSERT(i != 0)
        
        // test exception handler
        i = 1 / i;
        
        // should not get here
        panic("tick tick tick BOOM! :-)");
	return 0;
}
