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
 */

device_t* pm_name2device(char *name)
{
        device_t *dev = devices_head;
        
        if (dev == NULL)
                return NULL;
        
        do {
                if (strcmp(dev->name, name) == 0)
                        return dev;
                
                dev = dev->next;
        } while (dev != NULL);
        
        return NULL;
}

device_t* pm_fd2device(int fd)
{
        device_t *dev = devices_head;
        
        if (dev == NULL)
                return NULL;
        
        do {
                if (dev->fd == fd)
                        return dev;
                
                dev = dev->next;
        } while (dev != NULL);
        
        return NULL;
}

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
