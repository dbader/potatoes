#include "../include/types.h"
#include "../include/const.h"
#include "../include/string.h"
#include "../include/debug.h"
#include "../io/io_virtual.h"
#include "../io/io.h"
#include "pm_devices.h"

int dev_framebuffer_open(void *dev, char *path, int oflag, int mode)
{
        get_active_virt_monitor()->disable_refresh = TRUE;
        return ((device_t*)dev)->fd;
}

int dev_framebuffer_close(void *dev, int fd)
{
        get_active_virt_monitor()->disable_refresh = FALSE;
        return 0;
}

int dev_framebuffer_read(void *dev, int fd, void *buf, int size)
{
        return -1;
}

//* Draws ASCII "pixels" directly to the video memory without using the vmonitors.
int dev_framebuffer_write(void *dev, int fd, void *buf, int size)
{
        //TODO: skip rendering if the active vmonitor is different from the caller's
        if (size > 25 * 80) {
                size = 25 * 80;
        }
        
        uint16 *vid = (uint16*) 0xB8000;
        for (int i = 0; i < size; i++) {
                *vid++ = *((uint8*)buf++) * 0x1000 + ' ';
        }

        return size;
}

int dev_framebuffer_seek(void *dev, int fd, int offset, int whence)
{
        return -1;
}

device_t dev_framebuffer = {
        "/dev/framebuffer",    // name
        3,              // fd
        NULL,           // data
        dev_framebuffer_open,
        dev_framebuffer_close,
        dev_framebuffer_read,
        dev_framebuffer_write,
        dev_framebuffer_seek
};
