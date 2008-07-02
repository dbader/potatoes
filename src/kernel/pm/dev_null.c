#include "../include/types.h"
#include "../include/const.h"
#include "../include/string.h"
#include "../include/debug.h"
#include "pm_devices.h"

int dev_null_open(void *dev, char *path, int oflag, int mode)
{
        return ((device_t*)dev)->fd;
}

int dev_null_close(void *dev, int fd)
{
        return 0;
}

int dev_null_read(void *dev, int fd, void *buf, int size)
{
        dprintf("reading %d bytes from /dev/null\n", size);
        memset(buf, 0, size);
        return size;
}

int dev_null_write(void *dev, int fd, void *buf, int size)
{
        dprintf("writing %d bytes to /dev/null\n", size);
        return size;
}

int dev_null_seek(void *dev, int fd, int offset, int whence)
{
        return -1;
}

device_t dev_null = {
                "/dev/null",    // name
                0,              // fd
                NULL,           // data
                dev_null_open,
                dev_null_close,
                dev_null_read,
                dev_null_write,
                dev_null_seek
};
