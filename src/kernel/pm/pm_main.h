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

#ifndef __PM_MAIN_H
#define __PM_MAIN_H

#include "../include/ringbuffer.h"

/**
 * Process structure.
 * TODO: Maybe move this into pm_process.h
 */
typedef struct {
        /** process name */
        char *name;
        
        /** process id */
        uint32 pid;
        
        /** process state: running, dead, ... */ 
        sint8 state;
        
        /** memory address */
        void *addr;
        
        /** process timeslice */
        uint32 timeslice;
        
        /** STDIN queue */
        ring_fifo *stdin;
        
        /* machine_state ms; */
        /* xxx *stdout */
        /* llist *mem_blocks; */
        /* proc_file_table *pft; */
     
        /** linked list next ptr */
        struct process *next; 
} process_t;

void pm_init();
void pm_schedule();

#endif /* pm_main.h */
