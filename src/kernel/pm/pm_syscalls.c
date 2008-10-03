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
#include "pm_devices.h"

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

#define SYSCALL_TRACE //dprintf("[%u] ", getpid()); dprintf

void pm_syscall(uint32 id, void* data)
{
        if (id > MAX_SYSCALL)
                panic("pm_syscall: id > MAX_SYSCALL"); // more verbosity, ie pid, name, regs, stack
        set_interrupts(); //FIXME: I don't know why, but without this, it won't work in virtualbox
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

        free_virt_monitor(active_proc->vmonitor);
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

        char* path = strdup(args->path);
        
        device_t *dev = pm_name2device(path);
        if (dev != NULL) {
                // It's a device file
                //dprintf("opening device %s\n", dev->name);
                args->fd = dev->open(dev, path, args->oflag, args->mode);
        } else {
                // It's a real file.
                int fd = do_open(path);

                // Does not exist yet and the create flag ist set
                if (fd == NOT_POSSIBLE && args->oflag == O_CREAT) {
                        // See if it's a directory
                        if (path[strlen(path)-1] == '/') {
                                // Kill trailing slash.
                                path[strlen(path)-1] = '\0';
                                do_mkdir(path);
                                
                        } else
                                do_mkfile(path);

                        fd = do_open(path);
                }

                if (fd == NOT_POSSIBLE)
                        args->fd = -1;
                else {
                        args->fd = insert_proc_file(active_proc->pft, fd) + MAX_DEVICES;
                }
        }
        
        free(path);
}

void sys_close(void* data)
{
        sc_close_args_t *args = (sc_close_args_t*) data;
        SYSCALL_TRACE("SYS_CLOSE(%d)\n", args->fd);

        if (args->fd < 0) {
                args->success = -1;
                return;
        }

        if (args->fd < MAX_DEVICES) {
                // It's a device file
                device_t *dev = pm_fd2device(args->fd);
                if (!dev)
                        args->success = -1;
                else
                        args->success = dev->close(dev, args->fd);
        } else {
                if (do_close_pf(active_proc->pft, args->fd - MAX_DEVICES) == EOF)
                        args->success = -1;
                else
                        args->success = 0;
        }
}

/**
 * int _read(int fd, void *buf, int size);
 */
void sys_read(void* data)
{
        sc_read_write_args_t *args = (sc_read_write_args_t*) data;
        //SYSCALL_TRACE("SYS_READ(%d, 0x%x, %d)\n", args->fd, args->buf, args->size);

        if (args->fd < MAX_DEVICES) {
                // It's a device file
                device_t *dev = pm_fd2device(args->fd);
                if (!dev)
                        args->rw_count = -1;
                else
                        args->rw_count = dev->read(dev, args->fd, args->buf, args->size);
        } else {
                // It's a regular file
                SYSCALL_TRACE("SYS_READ(%d, 0x%x, %d)\n", args->fd, args->buf, args->size);
                proc_file *pft_entry = get_proc_file(active_proc->pft, args->fd - MAX_DEVICES);
                args->rw_count = do_read(pft_entry->pf_f_desc, args->buf, args->size, pft_entry->pf_pos);

                // Right now do_read() does not support partially successful reads.
                // So we have to patch that up:
                //if (args->rw_count != FALSE)
                //      args->rw_count = args->size;

                pft_entry->pf_pos += args->rw_count;
        }
}

void sys_write(void* data)
{
        sc_read_write_args_t *args = (sc_read_write_args_t*) data;
        SYSCALL_TRACE("SYS_WRITE(%d, 0x%x, %d)\n", args->fd, args->buf, args->size);

        //dprintf("writing to fd%d: ", args->fd);
        if (args->fd < MAX_DEVICES) {
                // It's a device file
                device_t *dev = pm_fd2device(args->fd);
                if (!dev)
                        args->rw_count = -1;
                else {
                        //  dprintf("write func of %s is at 0x%x\n", dev->name, dev->write);
                        args->rw_count = dev->write(dev, args->fd, args->buf, args->size);
                }
        } else {
                // It's a regular file
                proc_file *pft_entry = get_proc_file(active_proc->pft, args->fd - MAX_DEVICES);
                args->rw_count = do_write(pft_entry->pf_f_desc, args->buf, args->size, pft_entry->pf_pos);

                // Right now do_write() does not support partially successful writes.
                // So we have to patch that up:
                //if (args->rw_count != FALSE)
                //      args->rw_count = args->size;

                pft_entry->pf_pos += args->rw_count;
        }
}

void sys_seek(void* data)
{
        sc_seek_args_t *args = (sc_seek_args_t*) data;
        SYSCALL_TRACE("SYS_SEEK(%d, %d, %d)\n", args->fd, args->offset, args->whence);

        if (args->fd < MAX_DEVICES) {
                // It's a device file
                device_t *dev = pm_fd2device(args->fd);
                if (!dev)
                        args->pos = -1;
                else
                        args->pos = dev->seek(dev, args->fd, args->offset, args->whence);
        } else {
                // It's a regular file
                proc_file *pft_entry = get_proc_file(active_proc->pft, args->fd - MAX_DEVICES);

                switch (args->whence) {
                case SEEK_SET:
                        pft_entry->pf_pos = args->offset;
                        break;
                case SEEK_CUR:
                        pft_entry->pf_pos += args->offset;
                        break;
                case SEEK_END:
                        pft_entry->pf_pos = get_file(pft_entry->pf_f_desc)->f_inode->i_size + args->offset;
                        break;
                }

                args->pos = pft_entry->pf_pos;
        }
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
        //printf("SYS_FREE(0x%x)\n", data);
        free(data);
}
