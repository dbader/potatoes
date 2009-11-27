/* $Id: dev_stdout.c 266 2009-10-14 08:15:36Z dtraytel $
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
 * The STDOUT device. Allows a process to write text to the vmonitor
 * assigned to it.
 *
 * @author Daniel Bader
 * @author $LastChangedBy: dtraytel $
 * @version $Rev: 266 $
 */

#include "../include/types.h"
#include "../include/const.h"
#include "../include/string.h"
#include "../include/debug.h"
#include "../io/io_virtual.h"
#include "syscalls_cli.h"
#include "pm_devices.h"
#include "pm_main.h"

int dev_stdout_open(void *dev, char *path, int oflag, int mode)
{
        return ((device_t*)dev)->fd;
}

int dev_stdout_close(void *dev, int fd)
{
        return 0;
}

int dev_stdout_read(void *dev, int fd, void *buf, int size)
{
        return 0;
}

int dev_stdout_write(void *dev, int fd, void *buf, int size)
{
        char* temp = _malloc(size+1);
        temp[size] = '\0';
        memcpy(temp, buf, size);
        virt_monitor_puts(active_proc->vmonitor, temp);
        _free(temp);
        return size;
}

int dev_stdout_seek(void *dev, int fd, int offset, int whence)
{
        return -1;
}

device_t dev_stdout = {
        "/dev/stdout",  // name
        1,              // fd
        NULL,           // data
        dev_stdout_open,
        dev_stdout_close,
        dev_stdout_read,
        dev_stdout_write,
        dev_stdout_seek
};
