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
                sys_close,      // 4
                sys_read,       // 5
                sys_write,      // 6
                sys_seek,       // 7
                sys_malloc,     // 8
                sys_free        // 9
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
        sc_open_args_t *args = (sc_open_args_t*) data;
        SYSCALL_TRACE("SYS_OPEN(\"%s\", 0x%x, 0x%x)\n", args->path, args->oflag, args->mode);
        
        // If opening fails attempt to create the file
        int fd = do_open(args->path);
        
        if (fd < 0)
                do_create(args->path, 0); //TODO: '0' is neither DIRECTORY nor DATA_FILE!
        
        fd = do_open(args->path);
          
        args->fd = insert_proc_file(active_proc->pft, fd);
}

void sys_close(void* data)
{
        sc_close_args_t *args = (sc_close_args_t*) data;
        SYSCALL_TRACE("SYS_CLOSE(%d)\n", args->fd);
        
        if (do_close_pf(active_proc->pft, args->fd) == FALSE)
                args->success = -1;
        else
                args->success = 0;
}

/**
 * int _read(int fd, void *buf, int size); 
 */
void sys_read(void* data)
{
        sc_read_write_args_t *args = (sc_read_write_args_t*) data;
        SYSCALL_TRACE("SYS_READ(%d, 0x%x, %d)\n", args->fd, args->buf, args->size);
        
        proc_file *pft_entry = get_proc_file(active_proc->pft, args->fd); 
        args->rw_count = do_read(pft_entry->pf_f_desc, args->buf, args->size, pft_entry->pf_pos);
        
        // Right now do_read() does not support partially successful reads.
        // So we have to patch that up: 
        if (args->rw_count != FALSE)
                args->rw_count = args->size;
        
        pft_entry->pf_pos += args->rw_count;
}

void sys_write(void* data)
{
        sc_read_write_args_t *args = (sc_read_write_args_t*) data;
        SYSCALL_TRACE("SYS_WRITE(%d, 0x%x, %d)\n", args->fd, args->buf, args->size);

        proc_file *pft_entry = get_proc_file(active_proc->pft, args->fd);  
        args->rw_count = do_write(pft_entry->pf_f_desc, args->buf, args->size, pft_entry->pf_pos);

        // Right now do_write() does not support partially successful writes.
        // So we have to patch that up: 
        if (args->rw_count != FALSE)
                args->rw_count = args->size;

        pft_entry->pf_pos += args->rw_count;
}

void sys_seek(void* data)
{
        sc_seek_args_t *args = (sc_seek_args_t*) data;
        SYSCALL_TRACE("SYS_SEEK(%d, %d, %d)\n", args->fd, args->offset, args->whence);
        
        // TODO
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
