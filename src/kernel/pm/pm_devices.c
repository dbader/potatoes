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
 * Implementation of the device subsystem.
 *
 * @author dbader
 * @author $LastChangedBy$
 * @version $Rev$
 */

#include "../include/assert.h"
#include "../include/const.h"
#include "../include/debug.h"
#include "../include/init.h"
#include "../include/string.h"
#include "../fs/fs_const.h"
#include "../fs/fs_types.h"
#include "../fs/fs_file_table.h"
#include "../fs/fs_main.h"
#include "pm_devices.h"

/** Devices list. Holds all devices currently registered. */
device_t *devices_head = NULL;

/*
 * Possible devices:
 *      null
 *      zero
 *      log
 *      random
 *      hda
 *      mem
 *      clock
 *      stdin
 *      stdout
 *      stderr
 *      console?
 *      vmonitor?
 *      xx_status?
 *      brainfuck interpreter /dev/brainfuck
 *      framebuffer
 *      /dev/bombe
 *
 *
 * TODO:
stdout:
        - support for escape characters
                - control characters: \f clear screen \t \n \a \e

                - colors
                - cursor (-> needs changes in IO)

stdin:
        - support for escape characters
                - mouse cursor

/dev/mem_stats:
        total number of blocks
        total bytes allocated
        bytes free
        largest continuous free block
 */

/**
 * Resolves a device name string to its corresponding device_t pointer.
 * 
 * @param name the device name to resolve
 * @return the corresponding device structure or NULL if none could be found
 */
device_t* pm_name2device(char *name)
{
        device_t *dev = devices_head;

        if (dev == NULL) {
                return NULL;
        }

        do {
                if (strcmp(dev->name, name) == 0) {
                        return dev;
                }

                dev = dev->next;
        } while (dev != NULL);

        return NULL;
}

/**
 * Resolves a device handle to its corresponding device_t pointer.
 * 
 * @param fd the device handle to resolve
 * @return the corresponding device structure or NULL if none could be found
 */
device_t* pm_fd2device(int fd)
{
        device_t *dev = devices_head;

        if (dev == NULL) {
                return NULL;
        }

        do {
                if (dev->fd == fd) {
                        return dev;
                }

                dev = dev->next;
        } while (dev != NULL);

        return NULL;
}

/**
 * Registers a new device.
 * 
 * @param dev the device struct
 */
void pm_register_device(device_t *dev)
{
        if (pm_name2device(dev->name) != NULL) {
                printf("pm_register_device: %s already registered\n", dev->name);
                return;
        }

        if (pm_fd2device(dev->fd) != NULL) {
                printf("pm_register_device: fd %d already registered\n", dev->fd);
                return;
        }

        dev->next = devices_head;
        devices_head = dev;

        do_create(dev->name, 0);

        dprintf("%{pm:} registered device %s\n", VIOLET, dev->name);
}
