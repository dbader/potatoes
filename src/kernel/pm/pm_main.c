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
 * Process management main source file.
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

/** Process list. */
process_t *procs_head = NULL;

/** Pointer to the active process, ie the process which is executing. */
process_t *active_proc = NULL;

/** Pointer to the process which has the input focus. */
process_t *focus_proc = NULL;

/** Pointer to the kernel process. */
process_t *kernel_proc = NULL;

/** Process ID of the next process that gets created. */
uint32 next_pid = 0;

// Devices
extern device_t dev_null;
extern device_t dev_stdout;
extern device_t dev_stdin;
extern device_t dev_brainfuck;
extern device_t dev_framebuffer;
extern device_t dev_keyboard;
extern device_t dev_clock;

/**
 * Returns the PID of the active process.
 *
 * @return the PID
 */
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
        dprintf("#{VIO}pm:## init\n");
        dprintf("#{VIO}pm:## setting up kernel task...\n");

        procs_head = (process_t*) mallocn(sizeof(process_t),"pKERNEL PROC");

        memset(procs_head, 0, sizeof(process_t));

        // Initialize the kernel thread
        procs_head->name = "kernel";
        procs_head->state = PSTATE_ALIVE;
        procs_head->pid = next_pid++;
        procs_head->next = procs_head;
        init_proc_file_table(procs_head->pft);
        procs_head->stdin = rf_alloc(STDIN_QUEUE_SIZE);
        procs_head->vmonitor = get_active_virt_monitor();

        active_proc = procs_head;
        kernel_proc = procs_head;

        dprintf("#{VIO}pm:## %d syscalls registered\n", MAX_SYSCALL);

        dprintf("#{VIO}pm:## creating /dev\n");

        if (!do_file_exists("/dev")) {
                do_mkdir("/dev");
        }

        pm_register_device(&dev_null);
        pm_register_device(&dev_stdout);
        pm_register_device(&dev_stdin);
        pm_register_device(&dev_framebuffer);
        pm_register_device(&dev_keyboard);
        init_bf();
        pm_register_device(&dev_brainfuck);
        pm_register_device(&dev_clock);

        dprintf("#{VIO}pm:## scheduler initialized\n");
}

/**
 * Switch to the next process in a round robin fashion.
 *
 * @param context the tasks context on the stack
 * @return the context of the process which becomes active
 */
uint32 pm_schedule(uint32 context)
{
        // Bail out if the kernel task was not yet created by pm_init().
        if (active_proc == NULL) {
                return context;
        }

        active_proc->context = context;

        // Destroy all zombie- and jump over all sleeping processes up to the first alive process.
        while (active_proc->next->state != PSTATE_ALIVE) {
                while (active_proc->next->state == PSTATE_DEAD) {
                        process_t *next_proc = active_proc->next->next;
                        pm_destroy_thread(active_proc->next);
                        active_proc->next = next_proc;
                }
                //TODO: This is the PERFORMANCE FIX - schould be handled later in some other way
                while (active_proc->next->state == PSTATE_STDINSLEEP) {
                        active_proc = active_proc->next;
                }
        }

        active_proc = active_proc->next;
        return active_proc->context;
}

/**
 * Creates a new thread.
 *
 * @param name the process's name
 * @param entry the entry point
 * @param stacksize the stack size
 * @return the PID of the new thread
 */
uint32 pm_create_thread(char *name, void (*entry)(), uint32 stacksize)
{
        clear_interrupts();

        ASSERT(active_proc != NULL);
        ASSERT(name != NULL);
        ASSERT(entry != NULL);

        process_t *proc = mallocn(sizeof(process_t), name);

        if (proc == NULL) {
                panic("pm_create_thread: out of memory");
        }

        proc->name = strdup(name);
        proc->pid = next_pid++;
        proc->state = PSTATE_ALIVE;
        init_proc_file_table(proc->pft);
        proc->next = procs_head->next;
        procs_head->next = proc;

        proc->stack_start = mallocn(stacksize,"STACK");

        if (proc->stack_start == NULL) {
                panic("pm_create_thread: could not allocate stack");
        }

        proc->stdin = rf_alloc(STDIN_QUEUE_SIZE);

        if (proc->stdin == NULL) {
                panic("pm_create_thread: could not allocate stdin");
        }

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

        dprintf("#{VIO}pm:## created thread \"%s\"\n    "
                        "entry at 0x%x, stack at 0x%x (%u bytes). pid = %u\n\n",
                        proc->name, entry, proc->context, stacksize, proc->pid);

        focus_proc = proc; //FIXME: hackhackhack

        proc->vmonitor = start_vmonitor(name, proc->pid);

        set_interrupts();

        return proc->pid;
}

/**
 * Releases a thread's resources. The only place this should be called from
 * is pm_schedule() - to mark a process for destruction, set its "dead" flag.
 *
 * @see pm_schedule
 * @param proc the process to destroy
 */
void pm_destroy_thread(process_t *proc)
{
        dprintf("#{VIO}pm:## destroy thread \"%s\" pid = %u\n\n", proc->name, proc->pid);

        if (proc->stdin != NULL) {
                rf_free(proc->stdin);
        }
        free(proc->name);
        free(proc->stack_start);
        free(proc);
}

/**
 * Gives a process the input focus.
 *
 * @param pid the pid of the process receiving the focus
 */
void pm_set_focus_proc(uint32 pid)
{
        focus_proc = pm_get_proc(pid);
}

/**
 * Returns the process that belongs to the given pid.
 *
 * @param pid the pid
 * @return process that belongs to the given pid
 */
process_t* pm_get_proc(uint32 pid)
{
        process_t *p = procs_head;
        do {
                if (p->pid == pid)
                        break;
                p = p->next;
        } while (p != procs_head);

        return p;
}

void aprintf(char *fmt, ...)
{
        va_list arg_list;
        va_start(arg_list, fmt);

        //dprintf(fmt, arg);

        char buf[255];
        vsnprintf(buf, sizeof(buf), fmt, arg_list);

        char *msg = buf;

        virt_monitor_puts(active_proc->vmonitor, msg);
        va_end(arg_list);
}

/**
 * Prints some status information about all processes. Can be used as a crude form
 * of unix's "ps" command.
 */
void pm_dump()
{
        //aprintf("hello, world %s %s %c", 0, "hello", 'x');
        aprintf("#{VIO}PID##\tNAME\t\tCONTEXT\n");
        aprintf("#{VIO}----------------------------------##\n");
        process_t *p = procs_head;
        do {
                aprintf("#{VIO}%d##\t%s\t\t0x%x\n", p->pid, p->name, p->context);
                p = p->next;
        } while (p != procs_head);

}
