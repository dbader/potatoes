/* $Id$
********************************************************************************
* _____   ____ _______    _______ ____  ______  _____                          *
*|  __ \ / __ \__   __|/\|__   __/ __ \|  ____|/ ____|          Copyright 2008 *
*| |__) | |  | | | |  /  \  | | | |  | | |__  | (___              Daniel Bader *
*|  ___/| |  | | | | / /\ \ | | | |  | |  __|  \___ \           Vincenz Doelle *
*| |    | |__| | | |/ ____ \| | | |__| | |____ ____) |    Johannes Schamburger *
*|_|     \____/  |_/_/    \_\_|  \____/|______|_____/          Dmitriy Traytel *
*                                                                              *
*      Practical Oriented TeAching Tool, Operating (and) Educating System      *
*                                                                              *
*                           www.potatoes-project.tk                            *
*******************************************************************************/

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
#include "../../apps/shell_main.h"
#include "../io/io.h"
#include "../pm/pm_main.h"
#include "../mm/mm.h"
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
 *
 * Usage example: @code mem_start = (uint32) &end; @endcode
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

extern void shell_main();

/**
 * C kernel entry point.
 *
 * @param mboot_ptr The multiboot struct passed by the bootloader (grub).
 */
int main(struct multiboot *mboot_ptr)
{
        g_mboot_ptr = mboot_ptr;

        //mm_init((uint32)&end, 0x100000 + mboot_ptr->mem_upper * 1024);
        mm_init(0x300001, 0x100000 + mboot_ptr->mem_upper * 1024);
        init_vmonitors();
        mm_init_output();
        io_init();
        fs_init();
        pm_init();

        dprint_separator();
        printf("#{RED}main:## %d bytes kernel stack\n", 0x300000 - (uint32)&end);
        printf("#{RED}main:## init complete at %d ticks.\n", get_ticks());

        do_tests();

        printf("#{RED}main:## entering idle loop\n");

        // launch the shell.
        new_shell();

        // kernel idle loop
        for (;;)
                halt();

        return 0;
}
