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

        if (!do_file_exists(dev->name)) {
                do_create(dev->name, 0);
        }

        dprintf("#{VIO}pm:## registered device %s\n", dev->name);
}
