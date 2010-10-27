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

/**
 * Represents the state of all cpu registers at a given time.
 * Primarily used to provide additional information and error
 * locations to the user in the exception handlers.
 */
typedef struct {
        unsigned int gs, fs, es, ds;
        unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
        unsigned int int_no, err_code;
        unsigned int eip, cs, eflags, useresp, ss;
} /*__attribute__((__packed__))*/  cpu_state_t;

/** 
 * The process is alive, ie it will get executed in one of the
 * next pm_schedule() calls. 
 */
#define PSTATE_ALIVE 0

/**
 * The process is dead, ie it will get purged in one of the
 * next pm_schedule() calls.
 */
#define PSTATE_DEAD  1

/**
 * Process is sleeping, due to stdin requesting input, but none available. 
 */
#define PSTATE_STDINSLEEP  2

/**
 * Structure describing a single process.
 * TODO: Maybe move this into pm_process.h
 */
typedef struct process_t {
        /** a readable name string */
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

        /** number of consecutive timeslices the process is offered
         *  everytime it becomes active */
        uint32 priority;

        /** decremented every timer interrupt. The process becomes inactive when
         *  remaining_timeslices is 0 */
        uint32 remaining_timeslices;
} process_t;

/** Maximum number of input bytes the STDIN data structure can queue up */
#define STDIN_QUEUE_SIZE 512

extern process_t *procs_head;
extern process_t *active_proc;
extern process_t *focus_proc;
extern process_t *kernel_proc;

uint32 getpid();
void pm_init();
uint32 pm_schedule(uint32 context);
uint32 pm_create_thread(char *name, void (*entry)(), uint32 stacksize);
void pm_destroy_thread(process_t *proc);
process_t* pm_get_proc(uint32 pid);
void pm_set_thread_priority(uint32 pid, uint32 prio);
void pm_set_focus_proc(uint32 pid);
void pm_kill_proc(uint32 pid);
extern void _syscall(uint32 id, void *data);

#endif /* pm_main.h */
