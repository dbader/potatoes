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
 * The clock device. This provides access to the current time and date
 * in string form.
 *
 * @author Daniel Bader
 * @author $LastChangedBy$
 * @version $Rev$
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
