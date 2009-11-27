/* $Id: dev_stdin.c 266 2009-10-14 08:15:36Z dtraytel $
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
 * The STDIN device. Provides a process with access to its character
 * input queue.
 *
 * @author Daniel Bader
 * @author $LastChangedBy: dtraytel $
 * @version $Rev: 266 $
 */
 
#include "../include/types.h"
#include "../include/const.h"
#include "../include/string.h"
#include "../include/debug.h"
#include "pm_devices.h"
#include "pm_main.h"
#include "../include/ringbuffer.h"

int dev_stdin_open(void *dev, char *path, int oflag, int mode)
{
        return ((device_t*)dev)->fd;
}

int dev_stdin_close(void *dev, int fd)
{
        return 0;
}

int dev_stdin_read(void *dev, int fd, void *buf, int size)
{
        //TODO: This is the PERFORMANCE FIX
        while ((size > rf_getlength(active_proc->stdin)) && (active_proc != focus_proc)) {
                active_proc->state = PSTATE_STDINSLEEP;
                halt();
        }
        return rf_read(active_proc->stdin, buf, size);
}

int dev_stdin_write(void *dev, int fd, void *buf, int size)
{
        return rf_write(active_proc->stdin, buf, size);
}

int dev_stdin_seek(void *dev, int fd, int offset, int whence)
{
        return -1;
}

device_t dev_stdin = {
        "/dev/stdin",   // name
        2,              // fd
        NULL,           // data
        dev_stdin_open,
        dev_stdin_close,
        dev_stdin_read,
        dev_stdin_write,
        dev_stdin_seek
};
