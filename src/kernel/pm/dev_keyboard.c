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
 * The keyboard device. Provides direct access to the state of all
 * keys. Used to retrieve the state of keys which do not produce 
 * ASCII characters.
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

int dev_keyboard_open(void *dev, char *path, int oflag, int mode)
{
        return ((device_t*)dev)->fd;
}

int dev_keyboard_close(void *dev, int fd)
{
        return 0;
}

/* Provided by the IO driver */
extern bool keyboard_state[256];
int dev_keyboard_read(void *dev, int fd, void *buf, int size)
{
        if (!buf || !size)
                return -1;

        int to_copy = sizeof(keyboard_state);
        if (size < to_copy)
                to_copy = size;

        memcpy(buf, keyboard_state, to_copy);
        return to_copy;
}

int dev_keyboard_write(void *dev, int fd, void *buf, int size)
{
        return -1;
}

int dev_keyboard_seek(void *dev, int fd, int offset, int whence)
{
        return -1;
}

device_t dev_keyboard = {
        "/dev/keyboard",    // name
        4,              // fd
        NULL,           // data
        dev_keyboard_open,
        dev_keyboard_close,
        dev_keyboard_read,
        dev_keyboard_write,
        dev_keyboard_seek
};
