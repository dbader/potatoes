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

#include "../include/assert.h"
#include "../include/const.h"
#include "../include/stdio.h"
#include "../include/stdlib.h"
#include "../include/string.h"
#include "../include/debug.h"
#include "../include/ringbuffer.h"
#include "../io/io.h"
#include "pm_main.h"


process_t *procs_head = NULL;
process_t *active_proc = NULL;
process_t *focus_proc = NULL;

uint32 next_pid = 0;

/**
 * Performs process management initializations.
 */
void pm_init()
{
        dprint_separator();
        dprintf("pm: init\n");
        dprintf("pm: setting up kernel task...\n");
        
        procs_head = (process_t*) malloc(sizeof(process_t));
        
        procs_head->name = "kernel";
        procs_head->pid = next_pid++;
        procs_head->next = procs_head;
        
        active_proc = procs_head;
        
        dprintf("pm: scheduler initialized\n");
}
/**
 * Reschedule if needed.
 */
uint32 pm_schedule(uint32 context)
{
        // Bail out if the kernel task was not yet created by pm_init()
        if (active_proc == NULL) {
                return context;
        }

        active_proc->stack = context;
        active_proc = active_proc->next;
        return active_proc->stack;
}

uint32 pm_create_thread(char *name, void (*entry)(), uint32 stacksize)
{
        ASSERT(active_proc != NULL);
        ASSERT(name != NULL);
        ASSERT(entry != NULL);
        
        process_t *proc = malloc(sizeof(process_t));
        proc->name = strdup(name);
        proc->pid = next_pid++;
        proc->next = procs_head->next;
        procs_head->next = proc;
        
        proc->stack = (uint32) malloc(stacksize) + stacksize; // Stack grows downwards
        
        uint32 *stack = (uint32*) proc->stack;
        
        //
        // Setup the thread's initial stack
        //
        
        // Pushed by the cpu when an interrupt occurs
        *--stack = 0x202;          // EFLAGS
        *--stack = 0x08;           // CS
        *--stack = (uint32) entry; // EIP
        
        // IRQ stub data
        *--stack = 0;   //int no
        *--stack = 0;   //err code

        // General purpose regs (pushad)
        *--stack = 0;   // EDI
        *--stack = 0;   // ESI
        *--stack = 0;   // EBP
        *--stack = 0;   // NULL
        *--stack = 0;   // EBX
        *--stack = 0;   // EDX
        *--stack = 0;   // ECX
        *--stack = 0;   // EAX

        // Segment registers
        *--stack = 0x10; // DS
        *--stack = 0x10; // ES
        *--stack = 0x10; // FS
        *--stack = 0x10; // GS

        proc->stack = (uint32) stack;
        
        dprintf("pm: created thread \"%s\"\n    entry at 0x%x, stack at 0x%x (%u bytes). pid = %u\n\n",
                        proc->name, entry, proc->stack, stacksize, proc->pid);
        
        return proc->pid;
}

void pm_syscall(uint32 id, void* data)
{
        //dprintf("YIKES, SYSCALL! id %u, data 0x%x\n", id, data);
        putchar(id);
}

void printhex(uint32 hex)
{
        printf("0x%x\n", hex);
}
