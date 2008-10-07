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
 * The null device. This is just a dummy device. Use its source code
 * as an outline for devices you want to implement.
 *
 * @author Daniel Bader
 * @author $LastChangedBy$
 * @version $Rev$
 */
 
#include "../include/types.h"
#include "../include/const.h"
#include "../include/string.h"
#include "../include/debug.h"
#include "pm_devices.h"

int dev_null_open(void *dev, char *path, int oflag, int mode)
{
        return ((device_t*)dev)->fd;
}

int dev_null_close(void *dev, int fd)
{
        return 0;
}

int dev_null_read(void *dev, int fd, void *buf, int size)
{
        dprintf("reading %d bytes from /dev/null\n", size);
        memset(buf, 0, size);
        return size;
}

int dev_null_write(void *dev, int fd, void *buf, int size)
{
        dprintf("writing %d bytes to /dev/null\n", size);
        return size;
}

int dev_null_seek(void *dev, int fd, int offset, int whence)
{
        return -1;
}

device_t dev_null = {
        "/dev/null",    // name
        0,              // fd
        NULL,           // data
        dev_null_open,
        dev_null_close,
        dev_null_read,
        dev_null_write,
        dev_null_seek
};
