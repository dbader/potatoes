/* $Id: init.h 55 2008-05-15 20:08:02Z dtraytel $
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
 * @author $LastChangedBy: dtraytel $
 * @version $Rev: 55 $
 */
#include "../include/const.h"
#include "../include/types.h"
#include "../include/assert.h"
#include "../include/string.h"
#include "../include/stdio.h"

/**
 * output-testing
 */
void draw_test()
{ 
        puts_col("Test by Dmitriy Traytel (the i/o-master of etiOS)\n",WHITE,BLACK); 
        puts("      _   _  ____   _____\n"); 
        puts("     | | (_)/ __ \\ / ____|\n");
        puts("  ___| |_ _| |  | | (___\n");
        puts(" / _ \\ __| | |  | |\\___ \\\n"); 
        puts("|  __/ |_| | |__| |____) |\n");
        puts(" \\___|\\__|_|\\____/|_____/\n\n");
        puts_col("Copyright 2008 Daniel Bader,Vincenz Doelle,Johannes Schamburger,Dmitriy Traytel\n",0xA,BLACK);
        
        puts("\ttab-test\tok\n");
        puti(strlen("hallo welt")); //strlen-test
        
        //some ints
        puts("\t");
        puti(0);
        puts("\t");
        puti(100);
        puts("\t");
        puti(-2);
        puts("\t");
        
        //some hex-bytes
        uint8 *test = (uint8*)0x60;
        puthex(*test);                    //0x60h
        puthex(*(test + 1));              //0x61h
        puthex(*(test + 2));              //0x62h
        puthex(*(test + 3));              //0x63h
        puthex(*(test + 4));              //0x64h
        puthex(*(test + 5));              //0x65h
        
        puts("\t\n");
        puts("\tnumber-test\tok\n");
        
        puts("\tbegin color test:\n");
        puts_col("                                        ", BLACK, WHITE);
        puts_col("                                        ", BLACK, BLUE); 
        puts_col("                                        ", BLACK, GREEN); 
        puts_col("                                        ", BLACK, CYAN); 
        puts_col("                                        ", BLACK, RED); 
        puts_col("                                        ", BLACK, VIOLET);
        puts_col("                                        ", BLACK, ORANGE); 
        puts_col("                                        ", BLACK, LIGHTGREY); 
        puts_col("                                        ", BLACK, DARKGREY); 
        puts_col("                                        ", BLACK, LIGHTBLUE); 
        puts_col("                                        ", BLACK, LIGHTGREEN);
        puts_col("                                        ", BLACK, TURQUOISE); 
        puts_col("                                        ", BLACK, PINK); 
        puts_col("                                        ", BLACK, MAGENTA); 
        puts_col("                                        ", BLACK, YELLOW); 
        puts_col("                                        ", BLACK, BLACK);
        puts("\tcolor-test\tok\n");             
}

void printf_test()
{
        int x = 42;
        printf("this is a printf() test:\n");
        printf("hello %d and %d - 15%% done. - %s world!\n - %u - %c - %p\n\n",
                                23, x, "hello,", -17, '@', &x);
}

void strings_test()
{
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
}
// test ASSERT() macro
void assert_test()
{
        int i=0;
        ASSERT(TRUE)
        ASSERT(FALSE)
        ASSERT(i != 0)
}

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

/* just a hack */
#define MBOOT_INFO(x) puts("\n"); puts(#x); puts(": "); puti(mboot_ptr->x);

void grubstruct_test(struct multiboot *mboot_ptr)
{
        MBOOT_INFO(mem_lower)
        MBOOT_INFO(mem_upper)
        MBOOT_INFO(size)
        MBOOT_INFO(flags)
        MBOOT_INFO(addr)
        puts("\n\n");
}
