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
 * 
 *
 * @author dbader
 * @author $LastChangedBy$
 * @version $Rev$
 */

#include "../include/stdio.h"
#include "../include/stdlib.h"
#include "../include/debug.h"
#include "../include/ringbuffer.h"
#include "../io/io.h"
#include "pm_main.h"


process_t *procs_head;
process_t *active_proc;
process_t *focus_proc;

uint32 next_pid = 0;

/**
 * Performs process management initializations.
 */
void pm_init()
{
        dprintf("pm: init\n");
        
        procs_head = (process_t*) malloc(sizeof(process_t));
        
        procs_head->name = "kernel";
        procs_head->pid = next_pid++;
}
/**
 * Reschedule if needed.
 */
void pm_schedule()
{
        //dprintf("sched: %d\n", get_ticks());
}
