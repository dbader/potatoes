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
#define SYS_KILL        12

/** The highest syscall id that is still valid. Be sure to update this! */
#define MAX_SYSCALL     12

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
