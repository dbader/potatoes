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
 * The brainfuck device.
 *
 * @author Dmitriy Traytel
 * @author $LastChangedBy$
 * @version $Rev$
 */

#include "../include/types.h"
#include "../include/const.h"
#include "../include/string.h"
#include "../include/debug.h"
#include "pm_devices.h"
#include "../../apps/brainfuck_interpreter.h"

int dev_brainfuck_open(void *dev, char *path, int oflag, int mode)
{
        return ((device_t*)dev)->fd;
}

int dev_brainfuck_close(void *dev, int fd)
{
        return 0;
}

int dev_brainfuck_read(void *dev, int fd, void *buf, int size)
{
        for (int i = 0; i < size; i++)
                interpret_bf(*((char*)buf++));
        return 0;
}

int dev_brainfuck_write(void *dev, int fd, void *buf, int size)
{
        for (int i = 0; i < size; i++) {
                char ch = *((char*)buf + i);
                if (     ch != '.' &&
                                ch != ',' &&
                                ch != '+' &&
                                ch != '-' &&
                                ch != '<' &&
                                ch != '>' &&
                                ch != '[' &&
                                ch != ']')
                        continue;
                interpret_bf(ch);
        }
        return size;
}

int dev_brainfuck_seek(void *dev, int fd, int offset, int whence)
{
        return -1;
}

device_t dev_brainfuck = {
        "/dev/brainfuck",    // name
        BRAINFUCK,           // fd
        NULL,           // data
        dev_brainfuck_open,
        dev_brainfuck_close,
        dev_brainfuck_read,
        dev_brainfuck_write,
        dev_brainfuck_seek
};
