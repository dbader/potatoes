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
 * The STDOUT device. Allows a process to write text to the vmonitor
 * assigned to it.
 *
 * @author Daniel Bader
 * @author $LastChangedBy$
 * @version $Rev$
 */

#include "../include/types.h"
#include "../include/const.h"
#include "../include/string.h"
#include "../include/debug.h"
#include "../io/io_virtual.h"
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
        for (int i = 0; i < size; i++)
                virt_monitor_putc(active_proc->vmonitor, *((char*)buf++));
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
