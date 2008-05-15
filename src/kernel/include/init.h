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

/**
 * Multiboot structure. 
 * @see http://www.gnu.org/software/grub/manual/multiboot/multiboot.html
 */
struct multiboot {
   unsigned int flags;
   unsigned int mem_lower;
   unsigned int mem_upper;
   unsigned int boot_device;
   unsigned int cmdline;
   unsigned int mods_count;
   unsigned int mods_addr;
   unsigned int num;
   unsigned int size;
   unsigned int addr;
   unsigned int shndx;
   unsigned int mmap_length;
   unsigned int mmap_addr;
   unsigned int drives_length;
   unsigned int drives_addr;
   unsigned int config_table;
   unsigned int boot_loader_name;
   unsigned int apm_table;
   unsigned int vbe_control_info;
   unsigned int vbe_mode_info;
   unsigned int vbe_mode;
   unsigned int vbe_interface_seg;
   unsigned int vbe_interface_off;
   unsigned int vbe_interface_len;
}  __attribute__((packed));

//init-functions
 void gdt_init(); 
 void idt_init();
 void isr_init();
 void irq_init();
 void set_interrupts();

//test-functions
 void draw_test();
 void printf_test();
 void grubstruct_test(struct multiboot *mboot_ptr);
 void assert_test();
 void strings_test();
