/* $Id: int_handler.h 137 2008-06-15 22:04:35Z dbader $
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
 * The brainfuck device.
 *
 * @author Dmitriy Traytel
 * @author $LastChangedBy: dbader $
 * @version $Rev: 137 $
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
        for (int i = 0; i < size; i++){
                char ch=*((char*)buf++);
                if(     ch!='.' &&
                        ch!=',' &&
                        ch!='+' &&
                        ch!='-' &&
                        ch!='<' &&
                        ch!='>' &&
                        ch!='[' &&
                        ch!=']')
                        return 0;
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
