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
 * elf2flt
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
#include "../include/init.h"
#include "../io/io.h"
#include "../io/io_virtual.h"
#include "../fs/fs_const.h"
#include "../fs/fs_types.h"
#include "../fs/fs_file_table.h"
#include "../fs/fs_main.h"
#include "pm_main.h"
#include "pm_syscalls.h"
#include "syscalls_shared.h"
#include "pm_devices.h"

#include "../../apps/brainfuck_interpreter.h"

process_t *procs_head = NULL;
process_t *active_proc = NULL;
process_t *focus_proc = NULL;
uint32 next_pid = 0;

// Devices
extern device_t dev_null;
extern device_t dev_stdout;
extern device_t dev_stdin;
extern device_t dev_brainfuck;
extern device_t dev_framebuffer;
extern device_t dev_keyboard;
extern device_t dev_clock;

uint32 getpid()
{
        ASSERT(active_proc != NULL);
        return active_proc->pid;
}

/**
 * Performs process management initializations.
 */
void pm_init()
{
        dprint_separator();
        dprintf("%{pm:} init\n", VIOLET);
        dprintf("%{pm:} setting up kernel task...\n", VIOLET);
        
        procs_head = (process_t*) malloc(sizeof(process_t));
        
        memset(procs_head, 0, sizeof(process_t));
        
        procs_head->name = "kernel";
        procs_head->state = PSTATE_ALIVE;
        procs_head->pid = next_pid++;
        procs_head->next = procs_head;
        init_proc_file_table(procs_head->pft);
        procs_head->stdin = rf_alloc(STDIN_QUEUE_SIZE);
        
        active_proc = procs_head;
        
        dprintf("%{pm:} %d syscalls registered\n", VIOLET, MAX_SYSCALL);
        
        dprintf("%{pm:} creating /dev\n", VIOLET);
        do_mkdir("/dev");
        
        pm_register_device(&dev_null);
        pm_register_device(&dev_stdout);
        pm_register_device(&dev_stdin);
        pm_register_device(&dev_framebuffer);
        pm_register_device(&dev_keyboard);
       	init_bf();
        pm_register_device(&dev_brainfuck);
        pm_register_device(&dev_clock);
       
        dprintf("%{pm:} scheduler initialized\n", VIOLET);
        
        //test_ls();
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

        active_proc->context = context;
        
        // Destroy all zombie processes
        while (active_proc->next->state == PSTATE_DEAD) {
                pm_destroy_thread(active_proc->next);
                active_proc->next = active_proc->next->next;
        }
                
        active_proc = active_proc->next;
        return active_proc->context;
}

uint32 pm_create_thread(char *name, void (*entry)(), uint32 stacksize)
{
        clear_interrupts();
        
        ASSERT(active_proc != NULL);
        ASSERT(name != NULL);
        ASSERT(entry != NULL);
        
        process_t *proc = malloc(sizeof(process_t));
        
        if (proc == NULL)
                panic("pm_create_thread: out of memory");
        
        proc->name = strdup(name);
        proc->pid = next_pid++;
        proc->state = PSTATE_ALIVE;
        init_proc_file_table(proc->pft);
        proc->next = procs_head->next;
        procs_head->next = proc;
        
        proc->stack_start = malloc(stacksize);
        
        if (proc->stack_start == NULL)
                panic("pm_create_thread: could not allocate stack");
        
        proc->stdin = rf_alloc(STDIN_QUEUE_SIZE);
        
        if (proc->stdin == NULL)
                panic("pm_create_thread: could not allocate stdin");
        
        proc->context = (uint32) proc->stack_start + stacksize; // Stack grows downwards
        
        uint32 *stack = (uint32*) proc->context;
        
        //
        // Setup the thread's initial stack
        //
        
        // Pushed by the cpu when an interrupt occurs
        *--stack = 0x202;          // EFLAGS (make sure that interrupts are enabled)
        *--stack = 0x08;           // CS
        *--stack = (uint32) entry; // EIP
        
        // IRQ stub data
        *--stack = 0;   //int no
        *--stack = 0;   //err code

        // General purpose regs (pushad)
        *--stack = 0;   // EDI
        *--stack = 0;   // ESI
        *--stack = 0;   // EBP
        *--stack = 0;   // NULL (ESP?)
        *--stack = 0;   // EBX
        *--stack = 0;   // EDX
        *--stack = 0;   // ECX
        *--stack = 0;   // EAX

        // Segment registers
        *--stack = 0x10; // DS
        *--stack = 0x10; // ES
        *--stack = 0x10; // FS
        *--stack = 0x10; // GS

        proc->context = (uint32) stack;
        
        dprintf("%{pm:} created thread \"%s\"\n    "
                        "entry at 0x%x, stack at 0x%x (%u bytes). pid = %u\n\n",
                        VIOLET, proc->name, entry, proc->context, stacksize, proc->pid);
       
        focus_proc = proc; //FIXME: hackhackhack
        
        start_vmonitor(name);
        
        set_interrupts();
       
        
        return proc->pid;
}

void pm_destroy_thread(process_t *proc)
{
        dprintf("%{pm:} destroy thread \"%s\" pid = %u\n", VIOLET, proc->name, proc->pid);
        if (proc->stdin != NULL)
                rf_free(proc->stdin);
        free(proc->name);
        free(proc->stack_start);
        free(proc);
}

void pm_dump()
{
        dprintf("PID\tNAME\t\tCONTEXT\n");
        dprintf("----------------------------------\n");
        process_t *p = procs_head;
        do {
                dprintf("%d\t%s\t\t0x%x\n", p->pid, p->name, p->context);
                p = p->next;
        } while (p != procs_head);
        
}

void printhex(uint32 hex)
{
        printf("0x%x\n", hex);
}
