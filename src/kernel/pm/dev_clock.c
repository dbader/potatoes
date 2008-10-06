#include "../include/types.h"
#include "../include/const.h"
#include "../include/string.h"
#include "../include/stdlib.h"
#include "../include/debug.h"
#include "pm_devices.h"
#include "../io/io_rtc.h"

int dev_clock_open(void *dev, char *path, int oflag, int mode)
{
        return ((device_t*)dev)->fd;
}

int dev_clock_close(void *dev, int fd)
{
        return 0;
}

int dev_clock_read(void *dev, int fd, void *buf, int size)
{
        char* timestamp = mallocn(23, "dev_clock");
        strncpy(buf, time2str(timestamp), size);
        free(timestamp);
        return strlen(buf);
}

int dev_clock_write(void *dev, int fd, void *buf, int size)
{
        return -1;
}

int dev_clock_seek(void *dev, int fd, int offset, int whence)
{
        return -1;
}

device_t dev_clock = {
        "/dev/clock",    // name
        6,              // fd
        NULL,           // data
        dev_clock_open,
        dev_clock_close,
        dev_clock_read,
        dev_clock_write,
        dev_clock_seek
};
