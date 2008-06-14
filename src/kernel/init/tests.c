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
#include "../include/debug.h"

#include "../io/io_virtual.h"
#include "../io/io.h"
#include "../mm/mm.h"

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

// print a part of the main memory
void mm_print_memory(mm_header* mm_print_start)
{
        mm_header *ptr;
        for(ptr = (mm_header*) ((uint32)mm_print_start - sizeof(mm_header)); 
                ptr != mm_end->next; 
                ptr = ptr->next) {
                printf("%s (0x%x)\n", ptr->name, (uint32)ptr);
                printf("\tptr->next: 0x%x\n", ptr->next);
                printf("\tptr->prev: 0x%x\n", ptr->prev);
                printf("\tptr->size: 0x%x\n", ptr->size);

        }
}

void malloc_test()
{
        void *mm_test[10];
        uint32 free_start = free_memory();
        // total allocation amount: 2535 bytes + 6 * 16 bytes
        mm_test[1] = mallocn(100,"1");
        printf("1: allocating 100 bytes...\n");
        mm_test[2] = mallocn(100,"2");
        printf("2: allocating 100 bytes...\n");  
        mm_test[3] = mallocn(300,"3");
        printf("3: allocating 300 bytes...\n");
        mm_print_memory(mm_test[1]);
        free(mm_test[2]);
        printf("freeing 2 (100 bytes)...\n");
        mm_print_memory(mm_test[1]);
        mm_test[4] = mallocn(50,"4");
        printf("4: allocating 50 bytes...\n"); 
        mm_print_memory(mm_test[1]);
        mm_test[5] = mallocn(50,"5");
        printf("5: allocating 50 bytes...\n");
        mm_print_memory(mm_test[1]);
        mm_test[6] = mallocn(34,"6");
        printf("6: allocating 34 bytes...\n");
        mm_print_memory(mm_test[1]);
        printf("7: reallocating 3 to 300 bytes...\n");
        mm_test[7] = realloc(mm_test[3], 300);
        mm_print_memory(mm_test[1]);
        printf("8: reallocating 3 to 200 bytes...\n");
        mm_test[8] = realloc(mm_test[7], 200);
        mm_print_memory(mm_test[1]);
        printf("9: reallocating 3 to 301 bytes...\n");
        mm_test[9] = realloc(mm_test[8], 301);
        mm_print_memory(mm_test[1]);
        printf("\n");
        char *p = malloc(2000);
        p = "abc";
        printf("new allocated string: %s\n", p);
        mm_print_memory(mm_test[6]);
        uint32 free_end = free_memory();
        printf("free memory space: %d bytes\ntotal space allocated in mm_test(): %d bytes", 
                free_end, free_start - free_end);
        //free(mm_test[4]);
        //mdefrag();
        //mm_move_block(0x200000, (mm_header*)(mm_test[9] - sizeof(mm_header)));
        //mm_print_memory(mm_test[1]);
        
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
void hd_test()
{
        char src_string[] = "'foobar' visited HD on block 42 - that's great! - Thanks Dmitriy!!";
        hd_write_sector(42, (void*) src_string);

        char *res_string = malloc(50);
        hd_read_sector((void*) res_string, 42);
        puts(res_string);
        //hd_write_sector(42,(uint16*)0xB8000);
        //hd_read_sector((uint16*)0xB85A0,42);
}
extern void _syscall(uint16 id, void *data);
void syscall_test()
{
        void* test1 = malloc(1);
        void* test2 = malloc(2);
        void* test3 = malloc(1);
        printf("%p\t%p\t%p\n", test1, test2, test3);

        _syscall(23, test1);
        _syscall(42, test2);
        _syscall(3, test3);
}

void ralph_wiggum()
{
        printf("                                                                   \n");
        printf("                       #WWWWWWWW@                                  \n");
        printf("                   WWWWW# W@+W WWWWWW                              \n");
        printf("                @W  W#W  W W+ W *W WWWWW                           \n");
        printf("              @W  W W  +@ W  @    + W WW@W                         \n");
        printf("             WW  W W  +* W   W    W  * W#@WW                       \n");
        printf("           W W  W W   # #   W  #  *  @  W W#W                      \n");
        printf("          W W  @ *   W  W   W  W   W  W  W W WW                    \n");
        printf("         W W  W  @  W  W       W   W   *  * W WW                   \n");
        printf("        W W  W  W      W   W   W   @   W  W  W WW                  \n");
        printf("       W W  W  +   W       W   W    +      @  + W@                 \n");
        printf("      W  + #   W      @    W   W    W   W  W  W  W*                \n");
        printf("        W  W  +   W   W             W   #        ++#               \n");
        printf("     W *  W   W   W   W                           W W              \n");
        printf("    #  W  W   @                                   #  W             \n");
        printf("    W    +   @   @        WW +WW          #WWWW@   W               \n");
        printf("      W  W   W   W      +#      **       W      W  W               \n");
        printf("   @  @  W   W          W        @      W        W W               \n");
        printf("   W     +   *         +          @     W          W               \n");
        printf("   W #  +              W   #      W     W    WW    @               \n");
        printf("     W                 W  WW@     W     W    WW    @               \n");
        printf("     WW@               *   W      W     W        # @               \n");
        printf("    W+  #W              W                W       # W               \n");
        printf("                        +        W        W     #  W               \n");
        printf("   *  #*                  W    +W      *W+ +WW#    @               \n");
        printf("   * ++                    +WW#          *@        W               \n");
        printf("    @  *                        @         W        W               \n");
        printf("    W                           +  #      W         W              \n");
        printf("     W                          +W  W     W          W             \n");
        printf("      +WWW                     W+    W  #W           @             \n");
        printf("        W                     W      W@+              W            \n");
        printf("         W                   W     @#                 W            \n");
        printf("         W              @   @      @                  @            \n");
        printf("          W             W   W     W WWW              W             \n");
        printf("           W            WW@       W    W          *W               \n");
        printf("           *W          *   #            #      @WW                 \n");
        printf("          W  W@            +          W@WW###*W                    \n");
        printf("         #     W           +        @+    *   W                    \n");
        printf("         W      @W         *              W  #                     \n");
        printf("         W        *WW      *              W @                      \n");
        printf("         W           *WWWWWW            W @@ WW                    \n");
        printf("        # W              W  +W@        W W W   W*                  \n");
        printf("      @W  @W            W      W+      WW  @   WW                  \n");
        printf("     *      W           #       #W     W   W  W  *                 \n");
        printf("     W       W+        W          W    W    WW   W                 \n");
        printf("     +         W      W            W   W    W W  W                 \n");
        printf("                WW   W*             W +    W   W WW                \n");
        printf("                  #W#@              W WW  W     W* W               \n");
        printf("                   W#                W  WW       + +               \n");
        printf("     @            W                  W           W  W              \n");
        printf("     W                               W              +              \n");
        printf("     W                               W               W             \n");
        printf("    @ W                             +@               WW            \n");
        printf("   W  W                             W                 W            \n");
        printf("   +   W                           W                  WW           \n");
        printf("  W     W                         W                   @            \n");
        printf("  W     @+                       W                     WW          \n");
        printf("         W                      W                      W           \n");
        printf(" +        W*                  WW                       W W         \n");
}

void do_tests()
{
        printf("\n\ndo_tests():\n");
        //grubstruct_test(g_mboot_ptr);
        //strings_test();       
        //draw_test();
        //printf_test();
        //assert_test();
        //mm_print_memory();
        //strsep_test();
        //sleep_test();
        //hd_test();
        SHORTCUT_CTRL('1', assert_test);
        SHORTCUT_CTRL_SUPER('p', printf_test);
        SHORTCUT_CTRL('m', malloc_test);
        SHORTCUT_CTRL('s', syscall_test);
        SHORTCUT_CTRL('r', ralph_wiggum);

}
