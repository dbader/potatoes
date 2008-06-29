#include "../include/types.h"
#include "../include/const.h"
#include "../include/string.h"
#include "../include/debug.h"
#include "pm_devices.h"
#include "pm_main.h"
#include "../include/ringbuffer.h"

int dev_stdin_open(void *dev, char *path, int oflag, int mode)
{
        return ((device_t*)dev)->fd;
}

int dev_stdin_close(void *dev, int fd)
{
        return 0;
}

int dev_stdin_read(void *dev, int fd, void *buf, int size)
{
        return rf_read(active_proc->stdin, buf, size);
}

int dev_stdin_write(void *dev, int fd, void *buf, int size)
{
        return 0;
}

int dev_stdin_seek(void *dev, int fd, int offset, int whence)
{
        return -1;
}

device_t dev_stdin = {
                "/dev/stdin",   // name
                2,              // fd
                NULL,           // data
                dev_stdin_open,
                dev_stdin_close,
                dev_stdin_read,
                dev_stdin_write,
                dev_stdin_seek
};
