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
        puts("done.");

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
        puts("\n");
}

void malloc_test() {
        void *test;
        puts("malloc(23068671): allocation of 22 MB -> memory is full (10 of 32 MB are reserved for the kernel)\n");
        test = malloc(23068671);
        puti((uint32)test);
        puts("\nmalloc(1): allocation of 1 KB -> function returns pointer to address 0 (NULL)\n");
        test = malloc(1);
        puti((uint32)test);
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

void do_tests()
{
        puts("\n\ndo_tests():\n");
        grubstruct_test(g_mboot_ptr);
        //strings_test();       
        //draw_test();
        printf_test();
        //assert_test();
        //malloc_test();
        //strsep_test();
        //sleep_test();
}
