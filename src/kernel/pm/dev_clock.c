/* $Id: dev_clock.c 266 2009-10-14 08:15:36Z dtraytel $
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
 * The clock device. This provides access to the current time and date
 * in string form.
 *
 * @author Daniel Bader
 * @author $LastChangedBy: dtraytel $
 * @version $Rev: 266 $
 */
 
#include "../include/types.h"
#include "../include/const.h"
#include "../include/string.h"
#include "../include/stdlib.h"
#include "../include/debug.h"
#include "pm_devices.h"
#include "../io/io_rtc.h"

int dev_clock_open(void *dev, char *path, int oflag, int mode)
{
        return ((device_t*)dev)->fd;
}

int dev_clock_close(void *dev, int fd)
{
        return 0;
}

int dev_clock_read(void *dev, int fd, void *buf, int size)
{
        char* timestamp = mallocn(24, "dev_clock");
        strncpy(buf, time2str(time, timestamp), size);
        free(timestamp);
        return strlen(buf);
}

int dev_clock_write(void *dev, int fd, void *buf, int size)
{
        return -1;
}

int dev_clock_seek(void *dev, int fd, int offset, int whence)
{
        return -1;
}

device_t dev_clock = {
        "/dev/clock",    // name
        6,              // fd
        NULL,           // data
        dev_clock_open,
        dev_clock_close,
        dev_clock_read,
        dev_clock_write,
        dev_clock_seek
};
