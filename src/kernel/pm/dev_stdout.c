#include "../include/types.h"
#include "../include/const.h"
#include "../include/string.h"
#include "../include/debug.h"
#include "pm_devices.h"

int dev_stdout_open(void *dev, char *path, int oflag, int mode)
{
        return ((device_t*)dev)->fd;
}

int dev_stdout_close(void *dev, int fd)
{
        return 0;
}

int dev_stdout_read(void *dev, int fd, void *buf, int size)
{
        return 0;
}

int dev_stdout_write(void *dev, int fd, void *buf, int size)
{
        for (int i = 0; i < size; i++)
                putchar(*((char*)buf++));
        return size;
}

int dev_stdout_seek(void *dev, int fd, int offset, int whence)
{
        return -1;
}

device_t dev_stdout = {
        "/dev/stdout",  // name
        1,              // fd
        NULL,           // data
        dev_stdout_open,
        dev_stdout_close,
        dev_stdout_read,
        dev_stdout_write,
        dev_stdout_seek
};
