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
 * This contains all the syscall definitions for the "client" side. Every etiOS userspace
 * program needs this file to make system calls.
 * 
 * Most of the code here consists of simple stubs which convert between the function's
 * arguments and the argument format expected by the kernel.
 * 
 * See syscalls_shared.h for the definitions of the argument data structures (sc_XXX_args_t). 
 *
 * @author dbader
 * @author $LastChangedBy$
 * @version $Rev$
 */

#include "pm_main.h"
#include "syscalls_shared.h"

void _log(char* msg)
{
        _syscall(SYS_LOG, msg);
}

void _exit(int status)
{
        _syscall(SYS_EXIT, (void*)status);
        
        // Never return. 
        WAIT_FOR_INTERRUPT();
}

int _getpid()
{
        uint32 pid;
        _syscall(SYS_GETPID, &pid);
        return pid;
}

int _open(char *path, int oflag, int mode)
{
        sc_open_args_t args;
        args.path = path;
        args.oflag = oflag;
        args.mode = mode;
        _syscall(SYS_OPEN, &args);
        return args.fd;
}

int _read(int fd, void *buf, int size)
{
        sc_read_write_args_t args;
        args.fd = fd;
        args.buf = buf;
        args.size = size;
        _syscall(SYS_READ, &args);
        return args.rw_count;
}

int _write(int fd, void *buf, int size)
{
        sc_read_write_args_t args;
        args.fd = fd;
        args.buf = buf;
        args.size = size;
        _syscall(SYS_WRITE, &args);
        return args.rw_count;
}

int _close(int fd)
{
        sc_close_args_t args;
        args.fd = fd;
        _syscall(SYS_CLOSE, &args);
        return args.success;
}

void* _malloc(size_t size)
{
        sc_malloc_args_t args;
        args.size = size;
        _syscall(SYS_MALLOC, &args);
        return args.mem;
}

void _free(void *block)
{
        _syscall(SYS_FREE, block);
}
