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

typedef struct {
        unsigned int gs, fs, es, ds;
        unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
        unsigned int int_no, err_code;
        unsigned int eip, cs, eflags, useresp, ss;    
} /*__attribute__((__packed__))*/ cpu_state_t;

/**
 * Process structure.
 * TODO: Maybe move this into pm_process.h
 */
typedef struct process_t {
        /** process name */
        char *name;
        
        /** process id */
        uint32 pid;
        
        /** process state: running, dead, ... */ 
        uint8 state;
        
        uint32 stack;
        
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
        struct process_t *next; 
} process_t;

void pm_init();
uint32 pm_schedule(uint32 context);
uint32 pm_create_thread(char *name, void (*entry)(), uint32 stacksize);
extern void _syscall(uint32 id, void *data);

#endif /* pm_main.h */
