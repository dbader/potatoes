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
 * This is the kernel side implementation of the syscalls.  
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
#include "pm_main.h"
#include "syscalls_shared.h"
#include "../fs/fs_file_table.h"
#include "../fs/fs_main.h"
#include "../include/init.h"
#include "pm_syscalls.h"

/** 
 * The system call table. Make sure this corresponds to the constants
 * defined in syscall_shared.h (otherwise mayhem ensues). */
syscall_handler syscall_table[] = {
                sys_log,        // 0
                sys_exit,       // 1
                sys_getpid,     // 2
                sys_open,       // 3
                sys_read,       // 4
                sys_write,      // 5
                sys_close,      // 6
                sys_malloc,     // 7
                sys_free        // 8
};

//#define MAX_SYSCALL (sizeof(syscall_table) / sizeof(syscall_handler)) - 1

#define SYSCALL_TRACE dprintf("[%u] ", getpid()); dprintf

void pm_syscall(uint32 id, void* data)
{
        if (id > MAX_SYSCALL)
                panic("pm_syscall: id > MAX_SYSCALL"); // more verbosity, ie pid, name, regs, stack
        
        syscall_table[id](data);
}

/**
 * void log(char *msg)
 * Prints debug logging output to the console.
 * @param data arguments
 */
void sys_log(void *data)
{
        SYSCALL_TRACE("SYS_LOG('%s')\n", data);
        //printf("log [%u]: %s\n", getpid(), (char*)data);
        puts((char*)data);
}

/**
 * void _exit(int status)
 * Performs normal program termination. exit() marks the current process as terminated ("dead"),
 * informs any waiting parents but does not yet destroy the process. Freeing process resources
 * is done by pm_schedule().
 * @param data arguments
 */
void sys_exit(void *data)
{
        SYSCALL_TRACE("SYS_EXIT(%d)\n", (sint32)data);
        ASSERT(active_proc != NULL);
        
        if (getpid() == 0)
                panic("cannot exit() the kernel thread");
        
        dprintf("exit %u: %d\n", getpid(), (sint32)data);
        active_proc->state = PSTATE_DEAD; // pm_schedule will then unlink and destroy it.
        
        //TODO: return status to waiting parents
}

/**
 * int _getpid(); 
 */
void sys_getpid(void *data)
{
        SYSCALL_TRACE("SYS_GETPID(0x%x)\n", data);
        if (data == NULL)
                return;
        
        *((uint32*) data) = getpid();
}

/**
 * int _open(char *path, int oflag, ...); 
 */
void sys_open(void *data)
{
        SYSCALL_TRACE("SYS_OPEN(0x%x)\n", data);
        sc_open_args_t *args = (sc_open_args_t*) data;
        dprintf("open %s flags = 0x%x\n", args->path, args->oflag);
        
        // If opening fails attempt to create the file
        int fd = do_open(args->path);
        
        if (fd < 0)
                do_create(args->path, 0);
        
        fd = do_open(args->path);
        
        char *buf = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        printf("wrote %u bytes", do_write(fd, buf, strlen(buf), 0));
        
        args->fd = insert_proc_file(active_proc->pft, fd);
}

/**
 * int _read(int fd, void *buf, int size); 
 */
void sys_read(void* data)
{
        SYSCALL_TRACE("SYS_READ(0x%x)\n", data);
        sc_read_args_t *args = (sc_read_args_t*) data;
        dprintf("read fd = %u buf = 0x%x size = %d\n", args->fd, args->buf, args->size);
        
        proc_file *pft_entry = &active_proc->pft[args->fd]; 
        args->read = do_read(args->buf, pft_entry->pf_desc, args->size, pft_entry->pf_pos);
        pft_entry->pf_pos += args->read;
}

void sys_write(void* data)
{
        SYSCALL_TRACE("SYS_WRITE(0x%x)\n", data);
}

void sys_close(void* data)
{
        SYSCALL_TRACE("SYS_CLOSE(0x%x)\n", data);        
}

void sys_malloc(void *data)
{
        sc_malloc_args_t *args = (sc_malloc_args_t*) data;
        SYSCALL_TRACE("SYS_MALLOC(%u)\n", args->size);
        args->mem = mallocn(args->size, active_proc->name);
}

void sys_free(void *data)
{
        SYSCALL_TRACE("SYS_FREE(0x%x)\n", data);
        free(data);
}
