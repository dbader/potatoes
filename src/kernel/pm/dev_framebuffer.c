/* $Id: dev_framebuffer.c 266 2009-10-14 08:15:36Z dtraytel $
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
 * The framebuffer device. This provides a 80x25 pixels, 16 color video screen.
 * Video output through the framebuffer device is much faster as it
 * writes directly to the video memory and does not use the conversion
 * functions of the vmonitors. Used primarily for the various demo games.
 *
 * @author Daniel Bader
 * @author $LastChangedBy: dtraytel $
 * @version $Rev: 266 $
 */
 
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

int dev_framebuffer_write(void *dev, int fd, void *buf, int size)
{
        //TODO: skip rendering if the active vmonitor is different from the caller's
        
        /* Allow the rendered area to be smaller than full screen. */
        if (size > 25 * 80) {
                size = 25 * 80;
        }
        
        /* Write colored ASCII "space" characters to the video memory. */
        uint16 *vid = (uint16*) VGA_DISPLAY;
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
