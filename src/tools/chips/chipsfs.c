#include "chips.h"

#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdarg.h>

#include <sys/types.h>
#include <sys/stat.h>

static int chips_getattr(const char *path, struct stat *stbuf)
{
	memset(stbuf, 0, sizeof(struct stat));

	int fd = do_open((char *)path);

	if(fd < 0)
		return -ENOENT;

	potatoes_file_info_t info;
	get_file_info(fd, &info);

	if(info.mode == POTATOES_DIRECTORY)
	{
		stbuf->st_mode = S_IFDIR | 0777;
		stbuf->st_nlink = 2;
	}
	else
	{
		stbuf->st_mode = S_IFREG | 0666;
		stbuf->st_nlink = 1;
		stbuf->st_size = info.size;
	}

	do_close(fd);

	return 0;
}

static int chips_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
	int fd = do_open((char *)path);
	if(fd < 0)
		return -ENOENT;

	potatoes_file_info_t info;
	get_file_info(fd, &info);

	if(info.mode != POTATOES_DIRECTORY)
		return -ENOENT;

	potatoes_dir_entry dir[POTATOES_DIR_ENTRIES_PER_BLOCK];
	int count, pos = 0;
	int i;
	while((count = do_read(fd, dir, sizeof(dir), pos)) != 0)
	{
		for(i = 0; i < count/sizeof(dir[0]) && dir[i].inode != 0; ++i)
		{
			printf("%s\n", dir[i].name);
			filler(buf, dir[i].name, NULL, 0);
		}
		pos += count;
	}

	do_close(fd);

	return 0;
}

static int chips_open(const char *path, struct fuse_file_info *fi)
{
	int fd = do_open((char *)path);

	if(fd < 0)
		return -ENOENT;

	potatoes_file_info_t info;
	get_file_info(fd, &info);

	if(info.mode != POTATOES_DATA_FILE)
	{
		do_close(fd);
		return -ENOENT;
	}

	do_close(fd);
	return 0;
}

static int chips_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	int fd = do_open((char *)path);

	int ret = do_read(fd, buf, size, offset);

	do_close(fd);

	return ret;
}

static int chips_write (const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
	int fd = do_open((char *)path);

	int ret = do_write(fd, (char *)buf, size, offset);

	do_close(fd);

	return ret;
}

static int chips_release(const char *path, struct fuse_file_info *fi)
{
	return 0;
}

static int chips_truncate(const char *path, off_t offset)
{
	return fs_truncate((char *)path, offset) ? 0 : -1;
}

int chips_create(const char *path, mode_t mode, struct fuse_file_info *fi)
{
	if(chips_open(path, fi) == 0)
		return 0;

	if(!fs_create((char *)path, POTATOES_DATA_FILE))
		return -ENOENT;

	//chips_truncate(path, 0);

	return chips_open(path, fi);
}

int chips_mkdir(const char *path, mode_t mode)
{
	if(!fs_create((char *)path, POTATOES_DIRECTORY))
		return -ENOENT;

	return 0;
}

int chips_unlink(const char *path)
{
	int fd = do_open((char *)path);
	if(fd < 0)
		return -ENOENT;

	potatoes_file_info_t info;
	get_file_info(fd, &info);

	if(info.mode != POTATOES_DATA_FILE)
		return -EPERM;
	do_close(fd);

	return fs_delete((char *)path) ? 0 : -1;
}

static struct fuse_operations chips_oper = {
	.readdir = chips_readdir,
	.getattr = chips_getattr,
	.open = chips_open,
	.read = chips_read,
	.write = chips_write,
	.release = chips_release,
	.truncate = chips_truncate,
	.create = chips_create,
	.mkdir = chips_mkdir,
	.unlink = chips_unlink
};

int main(int argc, char **argv) {

	if(argc < 3)
	{
		fprintf(stderr, "Usage: %s <disk image> <mountpoint>\n", argv[0]);
		return 1;
	}

	PotatoesDisk *disk = potatoes_disc_create(argv[1]);

	if(disk == NULL)
	{
		fprintf(stderr, "Failed to open disk image %s\n", argv[1]);
		return 1;
	}

	potatoes_set_current_disk(disk);

	fs_init();

	return fuse_main(argc - 1, argv + 1, &chips_oper, NULL);
}
