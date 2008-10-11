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
 * Definitions for the device subsystem.
 *
 * @author dbader
 * @author $LastChangedBy$
 * @version $Rev$
 */
#ifndef __PM_DEVICES_H
#define __PM_DEVICES_H

// These are the functions every device must export.
typedef int (*dev_open_func)(void *dev, char *path, int oflag, int mode);
typedef int (*dev_close_func)(void *dev, int fd);
typedef int (*dev_read_func)(void *dev, int fd, void *buf, int size);
typedef int (*dev_write_func)(void *dev, int fd, void *buf, int size);
typedef int (*dev_seek_func)(void *dev, int fd, int offset, int whence);

/**
 * Describes a single device.
 */
typedef struct device_t {
        /** Device name, exported to the file system. */
        char name[20];
        
        /** 
         * The devices file descriptor. Note that this is global
         * and does not change from process to process. Make sure
         * it is unique.
         */
        int fd;
        
        /**
         * The data pointer can be used by a device to save
         * state information which is unique to the device instance.
         */
        void *data;
        
        dev_open_func open;
        dev_close_func close;
        dev_read_func read;
        dev_write_func write;
        dev_seek_func seek;
        
        /** Linked list next pointer. */
        struct device_t *next;
} device_t;

/**
 * Maximum number of registered devices. All file descriptors below this number
 * are assumed to be device handles. 
 */
#define MAX_DEVICES 100

device_t* pm_name2device(char *name);
device_t* pm_fd2device(int fd);
void pm_register_device(device_t *dev);

#endif /*__PM_DEVICES_H*/
