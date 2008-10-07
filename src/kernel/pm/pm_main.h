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
 * Process management main header file.
 *
 * @author dbader
 * @author $LastChangedBy$
 * @version $Rev$
 */

//FIXME: This is a mess. Cleanup needed.

#ifndef __PM_MAIN_H
#define __PM_MAIN_H

#include "../include/ringbuffer.h"
#include "../fs/fs_types.h"
#include "../fs/fs_const.h"
#include "../fs/fs_const.h"
#include "../io/io_virtual.h"

typedef struct {
        unsigned int gs, fs, es, ds;
        unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
        unsigned int int_no, err_code;
        unsigned int eip, cs, eflags, useresp, ss;
} /*__attribute__((__packed__))*/  cpu_state_t;

#define PSTATE_ALIVE 0
#define PSTATE_DEAD  1

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

        /** the memory area which constitutes the stack */
        uint32 context;

        /** pointer to the beginning of the memory that holds the stack */
        void *stack_start;

        /** memory address */
        void *addr;

        /** process timeslice */
        uint32 timeslice;

        /** STDIN queue */
        ring_fifo *stdin;

        /** process file table */
        proc_file pft[NUM_PROC_FILES];

		/** the virtual monitor this process is attached to */
        virt_monitor *vmonitor;

        /** linked list next ptr */
        struct process_t *next;
} process_t;

/** Maximum number of input bytes the STDIN data structure can queue up */
#define STDIN_QUEUE_SIZE 512

extern process_t *procs_head;
extern process_t *active_proc;
extern process_t *focus_proc;

uint32 getpid();

void pm_init();
uint32 pm_schedule(uint32 context);
uint32 pm_create_thread(char *name, void (*entry)(), uint32 stacksize);
void pm_destroy_thread(process_t *proc);
void pm_set_focus_proc(uint32 pid);
extern void _syscall(uint32 id, void *data);

#endif /* pm_main.h */
