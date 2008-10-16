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
 * This holds definitions and things which need to be shared between the kernel side of
 * the syscall subsystem and the client side.
 *
 * @author dbader
 * @author $LastChangedBy$
 * @version $Rev$
 */

#ifndef __SYSCALLS_SHARED_H
#define __SYSCALLS_SHARED_H

#include "../fs/fs_types.h"
#include "../io/io_rtc.h"

/** Stops execution until the next interrupt occurs. */
#define WAIT_FOR_INTERRUPT() __asm__("hlt");

/* These are readable definitions of the syscall ids. */
#define SYS_LOG         0
#define SYS_EXIT        1
#define SYS_GETPID      2
#define SYS_OPEN        3
#define SYS_CLOSE       4
#define SYS_READ        5
#define SYS_WRITE       6
#define SYS_SEEK        7
#define SYS_MALLOC      8
#define SYS_FREE        9
#define SYS_UNLINK      10
#define SYS_STAT        11

/** The highest syscall id that is still valid. Be sure to update this! */
#define MAX_SYSCALL     11

// Definitions for open()'s oflag argument
/** 
 * Attempts to open a file. If the file does not exists open() aborts
 * and returns -1. 
 */
#define O_OPEN  0

/** 
 * Attempts to open a file. If the file does not exist yet open()
 * creates the file and opens it. 
 */
#define O_CREAT 1

/** Arguments for the OPEN syscall. */
typedef struct sc_open_args_t {
        int fd; // return value
        char *path;
        int oflag;
        int mode;
} sc_open_args_t;

/** Arguments for the CLOSE syscall. */
typedef struct sc_close_args_t {
        int success; // return value
        int fd;
} sc_close_args_t;

/** Arguments for the READ and WRITE syscalls. */
typedef struct sc_read_write_args_t {
        int rw_count; // return value
        int fd;
        void *buf;
        int size;
} sc_read_write_args_t;

// Definitions for seek()'s whence argument
/** Seek to an absolute position. */
#define SEEK_SET 0

/** Seek relative from the current position of the file pointer. */ 
#define SEEK_CUR 1

/** Seek relative to the end of the file. */
#define SEEK_END 2

/** Arguments for the SEEK syscall. */
typedef struct sc_seek_args_t {
        int pos; // return value
        int fd;
        int offset;
        int whence;
} sc_seek_args_t;

/** Arguments for the MALLOC syscall. */
typedef struct sc_malloc_args_t {
        void *mem; // return value
        int size;
} sc_malloc_args_t;

/** Arguments for the UNLINK syscall. */
typedef struct sc_unlink_args_t {
        int success; // return value
        char *path;
} sc_unlink_args_t;

/** Stat structure returned by the STAT syscall. @see file_info_t */
typedef file_info_t stat;

/** Arguments for the STAT syscall. */
typedef struct sc_stat_args_t {
        int success; // return value
        stat *buf;
        char *path;
} sc_stat_args_t;

#endif /* syscalls_shared.h */
