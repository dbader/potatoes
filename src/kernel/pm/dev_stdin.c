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
 * The STDIN device. Provides a process with access to its character
 * input queue.
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
