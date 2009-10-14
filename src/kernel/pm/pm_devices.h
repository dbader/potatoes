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
