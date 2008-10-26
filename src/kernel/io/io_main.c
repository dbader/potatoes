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
