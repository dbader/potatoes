#ifndef PM_DEVICES_H_
#define PM_DEVICES_H_

typedef int (*dev_open_func)(void *dev, char *path, int oflag, int mode);
typedef int (*dev_close_func)(void *dev, int fd);
typedef int (*dev_read_func)(void *dev, int fd, void *buf, int size);
typedef int (*dev_write_func)(void *dev, int fd, void *buf, int size);
typedef int (*dev_seek_func)(void *dev, int fd, int offset, int whence);

typedef struct device_t {
        char name[20];
        int fd;
        void *data;
        dev_open_func open;
        dev_close_func close;
        dev_read_func read;
        dev_write_func write;
        dev_seek_func seek;
        struct device_t *next;
} device_t;

#define MAX_DEVICES 100

device_t* pm_name2device(char *name);
device_t* pm_fd2device(int fd);
void pm_register_device(device_t *dev);

#endif /*PM_DEVICES_H_*/
