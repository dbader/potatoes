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
//#include "../fs/fs_main.h"
#include "../pm/pm_main.h"
#include "../pm/syscalls_cli.h"
#include "../pm/syscalls_shared.h"

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
        char t[] = "ThisBLABLA";
        char t2[] = "isaSTRING.H";
        char t3[] = "test.";
        bzero(f, sizeof(f));
        strncat(f, t, 5);
        strcat(f, t2);
        strcat(f, t3);
        printf(f);
        //monitor_puts(f); monitor_puts("\n\n");


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
     
        // monitor_puts("done.");

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
void mm_print_memory()
{
        mm_header *ptr;
        for(ptr = mm_start->next; ptr != mm_end->next; ptr = ptr->next) {
                printf("%s (0x%x)", ptr->name, (uint32)ptr);
                printf("\tnext: 0x%x", ptr->next);
                printf("\tprev: 0x%x", ptr->prev);
                printf("\tsize: 0x%x\n", ptr->size);

        }
}

void malloc_test()
{
        void *mm_test[15];
        uint32 free_start = free_memory();
        // total allocation amount: 2535 bytes + 6 * 16 bytes
        mm_test[1] = mallocn(100,"1");
        printf("1: allocating 100 bytes...\n");
        mm_test[2] = mallocn(100,"2");
        printf("2: allocating 100 bytes...\n");  
        mm_test[3] = mallocn(300,"3");
        printf("3: allocating 300 bytes...\n\n");
        mm_print_memory();
        free(mm_test[2]);
        printf("\nfreeing 2 (100 bytes)...\n\n");
        mm_print_memory();
        printf("\ntrying to free 2 again...\n\n");
        free(mm_test[2]);
        mm_print_memory();
        printf("\ntrying to free the block at 3 + 10 bytes...\n\n");
        free((void*)(mm_test[3] + 10));
        mm_print_memory();
        mm_test[4] = mallocn(50,"4");
        printf("\n4: allocating 50 bytes...\n\n"); 
        mm_print_memory();
        mm_test[5] = mallocn(50,"5");
        printf("\n5: allocating 50 bytes...\n\n");
        mm_print_memory();
        mm_test[6] = mallocn(34,"6");
        printf("\n6: allocating 34 bytes...\n\n");
        mm_print_memory();
        printf("\n7: reallocating 3 to 300 bytes...\n\n");
        mm_test[7] = realloc(mm_test[3], 300);
        mm_print_memory();
        printf("\n8: reallocating 3 to 200 bytes...\n\n");
        mm_test[8] = realloc(mm_test[7], 200);
        mm_print_memory();
        printf("\n9: reallocating 3 to 301 bytes...\n\n");
        mm_test[9] = realloc(mm_test[8], 301);
        mm_print_memory();
        printf("\n");
        mm_print_memory();
        printf("\n10: callocn(5, sizeof(134), \"calloc\");\n\n");
        mm_test[10] = callocn(5, sizeof(134), "calloc");
        uint8* tmp;
        printf("content of 10: ");
        for(tmp = (uint8*)mm_test[10]; (uint32)tmp < (uint32)mm_test[10] + (5 * sizeof(134)); tmp++) {
                printf("%d", *tmp);
        }
        printf("\n\n");
        mm_print_memory();
        uint32 free_end = free_memory();
        printf("free memory space: %d bytes\ntotal space allocated in mm_test(): %d bytes", 
                free_end, free_start - free_end);
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

void hd_write_test()
{
        void *ptr = mallocn(512,"HD-Test"), *temp;
        ASSERT(ptr!=0);
        char ch=1;
        for(temp = ptr; temp < ptr+512; temp++){
                *(uint8*)temp = ch;
                if(ch==127) ch=1;
                else ch++;
        }
        hd_write_sector(500,ptr);
                printf("written!\n");
        free(ptr);
}

void hd_stresswrite_test()
{
        void *ptr = mallocn(512,"HD-Test"), *temp;
        char ch=15;
        for(temp = ptr; temp < ptr+512; temp++){
                *(uint8*)temp = ch;
                if(ch==127) ch=15;
                else ch++;
        }
        ASSERT(ptr!=0);
        int i;
        for(i = 0; i < 100; i++){
                hd_write_sector(i+500,ptr);
                printf("written!\t%s\n\n",(char*)ptr);
        }
        hd_write_sector(maxaddr, ptr);
        printf("written max\t%s\n\n",(char*)ptr);
        free(ptr);
}

void hd_stressread_test()
{
        void *ptr = mallocn(512,"HD-Test"), *temp;
        char ch=0;
        for(temp = ptr; temp < ptr+512; temp++){
                *(uint8*)temp = ch;
                ch = (ch + 1) % 128;
        }
        ASSERT(ptr!=0);
        int i;
        for(i = 0; i < 100; i++){
                hd_read_sector(ptr,i+500);
                ((uint8*)ptr)[i+11]=0;
                printf("%s\n\n",(char*)ptr);
        }
        hd_read_sector(ptr, maxaddr);
        ((uint8*)ptr)[10]=0;
        printf("read max\t%s\n\n",(char*)ptr);
        free(ptr);
}

int fgetch(int fd)
{
        int ch;
        while (_read(fd, &ch, sizeof(ch)) == 0)
                halt();
        return ch;
}

char* fgets(char *s, int n, int fd)
{
        char ch = 0;
        while ((n-- > 0) && (ch != '\n')) {
                ch = fgetch(fd);
                *s++ = ch;
        }
        return s;
                
}

void syscall_test_thread()
{
        // log
        _log("log() test.\n");
        
        // getpid
        printf("getpid() returns %u\n", _getpid());
        
        // Test open, write, close
        int fd = _open("/test", 0, 0);
        printf("fd = %u\n", fd);
        char buf[] = "Hello, World.";
        _write(fd, buf, strlen(buf) + 1);
        printf("current pos is %d\n", _seek(fd, SEEK_CUR, 0));
        _close(fd);
        
        // Test open, read, close
        char rbuf[50];
        memset(rbuf, 0, sizeof(rbuf));
        fd = _open("/test", 0, 0);
        printf("fd = %u\n", fd);
        _read(fd, rbuf, 5);
        _close(fd);
        _log(rbuf);
        
        // Test device files
        fd = _open("/dev/null", 0, 0);
        printf("devnull fd = %u\n", fd);
        uint32 data;
        _read(fd, &data, sizeof(data));
        printf("data = 0x%x\n", data);
        printf("current pos is %d\n", _seek(fd, SEEK_CUR, 0));
        
        // stdout
        int stdout = _open("/dev/stdout", 0, 0);
        printf("stdout = %u\n", stdout);
        _write(stdout, buf, strlen(buf) + 1);
        
        //stdin
        int stdin = _open("/dev/stdin", 0, 0);
        printf("please enter something: ");
        char input[32];
        memset(input, 0, sizeof(input));
        fgets(input, sizeof(input), stdin);
        printf("\nThank you. You entered: %s\n", input);
        _close(stdin);
        
        // malloc and free
        void *mem = _malloc(4096);
        printf("malloc() returns 0x%x\n", mem);
        _free(mem);
        
        // exit
        _exit(0);
}

void syscall_test()
{
        pm_create_thread("syscall_test", syscall_test_thread, 4096);
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

void create_fs();

void fs_tests()
{
        create_fs();
}

void isr_test()
{
        printf("isr_test: Raising int 3...\n");
        __asm__ ("int $0x3");
}

void threadA()
{
        _log("hello from task A\n");
                
        for(;;) {
                for (int i = 0; i < 9999; i++) ;
                _log("A");
        }
}

void threadB()
{
        _log("hello from task B\n"); // log()
        
        for(;;) {
                for (int i = 0; i < 9999; i++) ;
                _log("B");
        }
}

void threadA_test()
{
        pm_create_thread("test-A", threadA, 4096);        
}

void threadB_test()
{
        pm_create_thread("test-B", threadB, 4096);        
}

void nullptr_test()
{
        uint32 *ptr = NULL;
        putchar(*ptr);
}

void do_tests()
{
        printf("\n%{Global keyboard shortcuts:}\n", LIGHTGREEN);
        //fs_tests();
        //grubstruct_test(g_mboot_ptr);
        //strings_test();       
        //draw_test();
        //printf_test();
        //assert_test();
        //mm_print_memory();
        //strsep_test();
        //sleep_test();
        //hd_test();
        //hd_stresswrite_test();
        //hd_stressread_test();
        
        //SHORTCUT_CTRL('i', isr_test);
        //SHORTCUT_CTRL('1', assert_test);
        //SHORTCUT_CTRL_SUPER('p', printf_test);
        //SHORTCUT_CTRL('m', malloc_test);
        //SHORTCUT_CTRL('h', hd_stresswrite_test);
        //SHORTCUT_CTRL_SUPER('h', hd_stressread_test);
        //SHORTCUT_CTRL('s', syscall_test);
        SHORTCUT_CTRL('r', ralph_wiggum);
        //SHORTCUT_CTRL('f', fs_tests);
        SHORTCUT_CTRL('a', threadA_test);
        SHORTCUT_CTRL('b', threadB_test);
        //SHORTCUT_CTRL('n', nullptr_test);
        SHORTCUT_CTRL('+', switch_monitor_up);
        SHORTCUT_CTRL('-', switch_monitor_down);
        dprint_separator();
        //fs_tests();
}
