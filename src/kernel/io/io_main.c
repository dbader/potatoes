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
#include "../io/io_virtual.h"
#include "../include/const.h"
#include "../include/debug.h"

void io_init()
{        
        dprintf("-----------------------------------------------"
                        "---------------------------------io: init\n");
        idt_init();                     dprintf("io: IDT initialized\n");
        isr_init();                     dprintf("io: ISRs initialized\n");
        irq_init();                     dprintf("io: IRQs initialized\n");

        printf("0x%p\n",(get_active_virt_monitor())->begin);
        printf("0x%p\n",(get_active_virt_monitor())->vis_begin);
        printf("0x%p\n",(get_active_virt_monitor())->begin+VIRTUAL_MONITOR_SIZE);
        //start_shell();
        monitor_init();                 dprintf("io: monitor initialized\n");
        timer_init(FREQUENCY);          dprintf("io: timer initialized (%dHz)\n", FREQUENCY);
        set_interrupts();               dprintf("io: interrupts enabled\n");
        hd_init();                      dprintf("io: hard disk initialized\n");
        dprintf("io: initialized\n-----------------------------------------------"
                        "---------------------------------\n");
}
