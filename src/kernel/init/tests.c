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
#include "../include/const.h"
#include "../include/types.h"
#include "../include/assert.h"
#include "../include/string.h"
#include "../include/stdio.h"
#include "../include/stdlib.h"
#include "../include/init.h"
#include "../include/util.h"

/**
 * output-testing
 */
void draw_test()
{ 
        monitor_cputs("Test by Dmitriy Traytel (the i/o-master of etiOS)\n",WHITE,BLACK); 
        monitor_puts("      _   _  ____   _____\n"); 
        monitor_puts("     | | (_)/ __ \\ / ____|\n");
        monitor_puts("  ___| |_ _| |  | | (___\n");
        monitor_puts(" / _ \\ __| | |  | |\\___ \\\n"); 
        monitor_puts("|  __/ |_| | |__| |____) |\n");
        monitor_puts(" \\___|\\__|_|\\____/|_____/\n\n");
        monitor_cputs("Copyright 2008 Daniel Bader,Vincenz Doelle,Johannes Schamburger,Dmitriy Traytel\n",0xA,BLACK);
        
        monitor_puts("\ttab-test\tok\n");
        monitor_puti(strlen("hallo welt")); //strlen-test
        
        //some ints
        monitor_puts("\t");
        monitor_puti(0);
        monitor_puts("\t");
        monitor_puti(100);
        monitor_puts("\t");
        monitor_puti(-2);
        monitor_puts("\t");
        
        //some hex-bytes
        uint8 *test = (uint8*)0x60;
        monitor_puthex(*test);                    //0x60h
        monitor_puthex(*(test + 1));              //0x61h
        monitor_puthex(*(test + 2));              //0x62h
        monitor_puthex(*(test + 3));              //0x63h
        monitor_puthex(*(test + 4));              //0x64h
        monitor_puthex(*(test + 5));              //0x65h
        
        monitor_puts("\t\n");
        monitor_puts("\tnumber-test\tok\n");
        
        monitor_puts("\tbegin color test:\n");
        monitor_cputs("                                        ", BLACK, WHITE);
        monitor_cputs("                                        ", BLACK, BLUE); 
        monitor_cputs("                                        ", BLACK, GREEN); 
        monitor_cputs("                                        ", BLACK, CYAN); 
        monitor_cputs("                                        ", BLACK, RED); 
        monitor_cputs("                                        ", BLACK, VIOLET);
        monitor_cputs("                                        ", BLACK, ORANGE); 
        monitor_cputs("                                        ", BLACK, LIGHTGREY); 
        monitor_cputs("                                        ", BLACK, DARKGREY); 
        monitor_cputs("                                        ", BLACK, LIGHTBLUE); 
        monitor_cputs("                                        ", BLACK, LIGHTGREEN);
        monitor_cputs("                                        ", BLACK, TURQUOISE); 
        monitor_cputs("                                        ", BLACK, PINK); 
        monitor_cputs("                                        ", BLACK, MAGENTA); 
        monitor_cputs("                                        ", BLACK, YELLOW); 
        monitor_cputs("                                        ", BLACK, BLACK);
        monitor_puts("\tcolor-test\tok\n");             
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
        /* test some string stuff */
        char f[100];
        char t[] = "This BLABLA";
        char t2[] = "is a STRING.H";
        char t3[] = " test.";
        bzero(f, sizeof(f));
        strncat(f, t, 5);
        strcat(f, t2);
        strcat(f, t3);
        monitor_puts(f); monitor_puts("\n\n");
        
        
}

void strsep_test()
{
        char path[] = "/usr/share/bin/editor";
        char delim[] = "/";
        char *tok;
        char *copy = strdup(path);
        char *work_copy = copy;
      
        do {
                printf("strsep(\"%s\") ", work_copy);
                tok = strsep(&work_copy, delim);
                printf("-> \"%s\"\n", tok);
        } while (tok != NULL);

        printf("\ncopy = %p\n", copy);
        printf("work_copy = %p\n", work_copy);
        monitor_puts("done.");

        free(copy);
}

// test ASSERT() macro
void assert_test()
{
        int i=0;
        ASSERT(TRUE)
        ASSERT(FALSE)
        ASSERT(i != 0)
}

/* just a hack to easily dump mboot fields */
#define MBOOT_INFO(x) printf("\t%s = %u\n", #x, mboot_ptr->x); 

void grubstruct_test(struct multiboot *mboot_ptr)
{
        printf("mboot struct:\n");
        MBOOT_INFO(mem_lower)
        MBOOT_INFO(mem_upper)
        MBOOT_INFO(size)
        MBOOT_INFO(flags)
        MBOOT_INFO(addr)
        monitor_puts("\n");
}

void malloc_test() {
        void *test;
        monitor_puts("malloc(23068671): allocation of 22 MB -> memory is full (10 of 32 MB are reserved for the kernel)\n");
        test = malloc(23068671);
        monitor_puti((uint32)test);
        monitor_puts("\nmalloc(1): allocation of 1 KB -> function returns pointer to address 0 (NULL)\n");
        test = malloc(1);
        monitor_puti((uint32)test);
}

void sleep_test()
{
        printf("Sleep test...\n");
        sleep(1); printf(".");
        sleep(1); printf(".");
        sleep(1); printf(".");
        sleep(1); printf(".");
        sleep(1); printf(".");
        sleep(1); printf(".");
        sleep(1); printf(".");
        sleep(1); printf(".");
        sleep(1); printf(".");
        sleep(1); printf(".");
        printf("\n10 seconds later\n");
}

/**
 * Memcopy using hd ;-)
 */
void hd_test(){
        char src_string[] = "'foobar' visited HD on block 42 - that's great! - Thanks Dmitriy!!";
        hd_write_sector(42, (uint16*) src_string);
        
        char *res_string = malloc(50);
        hd_read_sector((uint16*) res_string, 42);
        puts(res_string);
	//hd_write_sector(42,(uint16*)0xB8000);
	//hd_read_sector((uint16*)0xB85A0,42);
}

void do_tests()
{
        monitor_puts("\n\ndo_tests():\n");
        //grubstruct_test(g_mboot_ptr);
        //strings_test();       
        //draw_test();
        //printf_test();
        //assert_test();
        //malloc_test();
        //strsep_test();
        //sleep_test();
        hd_test();
}
