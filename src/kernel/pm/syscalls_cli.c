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

/**
 * Writes a string to the kernel debug monitor. Useful to dump
 * strings that should not be displayed in the process's own
 * vmonitor.
 *
 * @param msg the text to print
 */
void _log(char* msg)
{
        _syscall(SYS_LOG, msg);
}

/**
 * Exits the calling process and releases its resources.
 *
 * @param status the return status
 */
void _exit(int status)
{
        _syscall(SYS_EXIT, (void*)status);

        // Never return.
        while(1) WAIT_FOR_INTERRUPT();
}

/**
 * Returns the PID (process id) of the calling process.
 *
 * @return the process id
 */
int _getpid()
{
        uint32 pid;
        _syscall(SYS_GETPID, &pid);
        return pid;
}

/**
 * Opens a file or a device.
 *
 * @param path the path of the file to open
 * @param oflag the open flag. @see O_OPEN @see O_CREAT
 * @param mode not used as of now. Set to 0.
 * @return a valid handle on success or -1 if failed
 */
int _open(char *path, int oflag, int mode)
{
        sc_open_args_t args;
        args.path = path;
        args.oflag = oflag;
        args.mode = mode;
        _syscall(SYS_OPEN, &args);
        return args.fd;
}

/**
 * Closes a open file handle.
 *
 * @param fd the file descriptor of the file to close
 * @return 0 on success, -1 on error
 */
int _close(int fd)
{
        sc_close_args_t args;
        args.fd = fd;
        _syscall(SYS_CLOSE, &args);
        return args.success;
}

/**
 * Reads data from a file or device.
 *
 * @param fd the file descriptor
 * @param buf the buffer to receive the data read
 * @param size the number of bytes to read
 * @return the number of bytes read or -1 on error
 */
int _read(int fd, void *buf, int size)
{
        sc_read_write_args_t args;
        args.fd = fd;
        args.buf = buf;
        args.size = size;
        _syscall(SYS_READ, &args);
        return args.rw_count;
}

/**
 * Writes data into a file or device.
 *
 * @param fd the file descriptor
 * @param buf the buffer containing the data to write
 * @param size the number of bytes to write
 * @return the number of bytes written or -1 on error
 */
int _write(int fd, void *buf, int size)
{
        sc_read_write_args_t args;
        args.fd = fd;
        args.buf = buf;
        args.size = size;
        _syscall(SYS_WRITE, &args);
        return args.rw_count;
}

/**
 * Moves the file pointer in a file or device. The file pointer
 * determines the current read and write position inside a file.
 *
 * @param fd the file descriptor
 * @param offset the offset from the position specified by the "whence" argument
 * @param whence the starting position of the seek operation.
 * 			@see SEEK_SET
 * 			@see SEEK_CUR
 * 			@see SEEK_END
 * @return the new position of the file pointer
 */
int _seek(int fd, int offset, int whence)
{
        sc_seek_args_t args;
        args.fd = fd;
        args.offset = offset;
        args.whence = whence;
        _syscall(SYS_SEEK, &args);
        return args.pos;
}

/**
 * Allocates new memory. @see _free
 *
 * @param size the number of bytes to allocate
 * @return a pointer to the new memory area or NULL if allocation failed
 */
void* _malloc(size_t size)
{
        sc_malloc_args_t args;
        args.size = size;
        _syscall(SYS_MALLOC, &args);
        return args.mem;
}

/**
 * Frees memory previously allocated through malloc(). @see _malloc
 *
 * @param block pointer to a memory block allocated through malloc
 */
void _free(void *block)
{
        _syscall(SYS_FREE, block);
}

/**
 * Deletes a filename and possible the file it refers to.
 *
 * @param path the filename to remove
 * @return zero on success, -1 on error
 */
int _unlink(char* path)
{
        sc_unlink_args_t args;
        args.path = path;
        _syscall(SYS_UNLINK, &args);
        return args.success;
}

/**
 * Returns extended file information for a single filename.
 *
 * @param path the filename
 * @param buf the stat structure to receive the results
 * @return zero on success, -1 on error
 */
int _stat(char *path, stat *buf)
{
        sc_stat_args_t args;
        args.path = path;
        args.buf = buf;
        _syscall(SYS_STAT, &args);
        return args.success;
}
