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
 * IO-init function for main
 *
 * @author Dmitriy Traytel
 * @author $LastChangedBy$
 * @version $Rev$
 */

#include "../io/io.h"
#include "../io/io_rtc.h"
#include "../io/io_virtual.h"
#include "../include/const.h"
#include "../include/debug.h"
#include "../include/string.h"

extern bool keyboard_state[256];
void io_init()
{
        dprint_separator();
        dprintf("#{YEL}io:## init\n");

        idt_init();
        dprintf("#{YEL}io:## IDT initialized\n");
        isr_init();
        dprintf("#{YEL}io:## ISRs initialized\n");
        irq_init();
        dprintf("#{YEL}io:## IRQs initialized\n");

        //dprintf("0x%p\n",(get_active_virt_monitor())->begin);
        //dprintf("0x%p\n",(get_active_virt_monitor())->vis_begin);
        //dprintf("0x%p\n",(get_active_virt_monitor())->begin+VIRTUAL_MONITOR_SIZE);


//        monitor_init();                 dprintf("io: monitor initialized\n");
        timer_init(FREQUENCY);
        dprintf("#{YEL}io:## timer initialized (%dHz)\n", FREQUENCY);
        set_interrupts();
        dprintf("#{YEL}io:## interrupts enabled\n");
        hd_init();
        dprintf("#{YEL}io:## hard disk initialized\n");
        memset(keyboard_state, FALSE, sizeof(keyboard_state));

        dprintf("#{YEL}io:## initialized\n");
}
