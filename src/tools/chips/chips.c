#include "chips.h"

#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>

static int chips_cat(int argc, char **argv)
{
	if(argc < 2)
	{
		fprintf(stderr, "Usage: %s <file> [more files ...]\n", argv[0]);
		return -1;
	}
	int i;
	for(i = 1; i < argc; ++i)
	{
		int fd = do_open(argv[i]);
		if(fd < 0)
		{
			fprintf(stderr, "Failed to open %s\n", argv[i]);
			return 1;
		}

		potatoes_file_info_t info;
		get_file_info(fd, &info);

		if(info.mode != POTATOES_DATA_FILE)
		{
			fprintf(stderr, "Not a file: %s\n", argv[i]);
			return 1;
		}

		char buf[512];
		int count;
		int off = 0;
		while((count = do_read(fd, buf, 512, off)) > 0)
		{
			off += count;
			write(1, buf, count);
		}
	}
	return 0;
}

static int chips_append(int argc, char **argv)
{
	if(argc != 2)
	{
		fprintf(stderr, "Usage: %s <file>\n", argv[0]);
		return -1;
	}
	int fd = do_open(argv[1]);
	if(fd < 0)
	{
		if(!fs_create(argv[1], POTATOES_DATA_FILE))	
		{
			fprintf(stderr, "Failed to create %s\n", argv[1]);
			return 1;
		}
		if((fd = do_open(argv[1])) < 0)
		{
			fprintf(stderr, "Failed to open %s\n", argv[1]);
			return 1;
		}
	}

	potatoes_file_info_t info;
	get_file_info(fd, &info);

	if(info.mode != POTATOES_DATA_FILE)
	{
		fprintf(stderr, "Not a file: %s\n", argv[1]);
		return 1;
	}

	char buf[512];
	int count;
	int off = info.size;
	while((count = read(0, buf, 512)) > 0)
	{
		do_write(fd, buf, count, off);
		off += count;
	}
	return 0;
}

static int chips_trunc(int argc, char **argv)
{
	if(argc < 2 || argc > 3)
	{
		fprintf(stderr, "Usage: %s <file> [size]\n", argv[0]);
		return -1;
	}
	int size = 0;
	if(argc == 3)
	{
		char *end;
		size = strtol(argv[2], &end, 0);
		if(*end != 0 || size < 0)
		{
			fprintf(stderr, "Not a nonnegative integer: %s\n", argv[2]);
			return -1;
		}
	}

	if(!fs_truncate(argv[1], size))
	{
		fprintf(stderr, "Truncating %s failed.\n", argv[1]);
		return 1;
	}
	return 0;
}

static int chips_ls(int argc, char **argv)
{
	char const *path = "/";
	if(argc > 1)
		path = argv[1];

	int fd = do_open((char *)path);
	if(fd < 0)
	{
		fprintf(stderr, "Failed to open %s\n", path);
		return -1;
	}

	potatoes_file_info_t info;
	get_file_info(fd, &info);

	if(info.mode != POTATOES_DIRECTORY)
	{
		printf("%s\n", info.name);
		return 0;
	}

	potatoes_dir_entry dir[POTATOES_DIR_ENTRIES_PER_BLOCK];
	int count = do_read(fd, dir, POTATOES_BLOCK_SIZE, 0) / POTATOES_DIR_ENTRY_SIZE;
	int i;
	for(i = 0; i < count && dir[i].inode != 0; ++i)
		printf("%s\n", dir[i].name);

	do_close(fd);

	return 0;
}

static int chips_mkdir(int argc, char **argv)
{
	if(argc != 2)
	{
		fprintf(stderr, "Usage: %s <path>\n", argv[0]);
		return -1;
	}
	if(!fs_create(argv[1], POTATOES_DIRECTORY))
	{
		fprintf(stderr, "Failed to create directory %s\n", argv[1]);
		return 1;
	}
	return 0;
}

static int chips_rm(int argc, char **argv)
{
	if(argc != 2)
	{
		fprintf(stderr, "Usage: %s <path>\n", argv[0]);
		return -1;
	}
	int fd = do_open(argv[1]);
	if(fd < 0)
	{
		fprintf(stderr, "Failed to open %s\n", argv[1]);
		return 1;
	}

	potatoes_file_info_t info;
	get_file_info(fd, &info);
	do_close(fd);

	if(info.mode != POTATOES_DATA_FILE)
	{
		fprintf(stderr, "Not a file: %s\n", argv[1]);
		return 1;
	}

	if(!fs_delete(argv[1]))
	{
		fprintf(stderr, "Failed to delete %s\n", argv[1]);
		return 1;
	}
	return 0;
}

typedef struct
{
	int (*exec)(int argc, char **argv);
	char const *name;
	char const *desc;
} command_t;

static const command_t chips_commands[] = {
	{chips_cat,    "cat",    "Dump contents of files to stdout."},
	{chips_append, "append", "Append stdin to a file, possibly creating it."},
	{chips_trunc,  "trunc",  "Set file size."},
	{chips_ls,     "ls",     "Show directory contents."},
	{chips_mkdir,  "mkdir",  "Create empty directory."},
	{chips_rm,     "rm",     "Delete a file."}
};

static const int chips_num_commands = sizeof(chips_commands)/sizeof(chips_commands[0]);

int chips_usage(char const *progname)
{
	fprintf(stderr, "Usage: %s <disk image> <command> [args ...]\n\n", progname);
	fprintf(stderr, "Available commands:\n");
	int i;
	for(i = 0; i < chips_num_commands; ++i)
		fprintf(stderr, "\t%-10s - %s\n", chips_commands[i].name, chips_commands[i].desc);
	return -1;
}

int main(int argc, char **argv)
{
	if(argc < 3)
		return chips_usage(argv[0]);

	PotatoesDisk *disk = potatoes_disc_create(argv[1]);

	if(disk == NULL)
	{
		fprintf(stderr, "Failed to open disk image %s\n", argv[1]);
		return 1;
	}

	potatoes_set_current_disk(disk);

	fs_init();

	int ret = -1;

	int i;
	for(i = 0; i < chips_num_commands; ++i)
		if(strcmp(argv[2], chips_commands[i].name) == 0)
		{
			ret = chips_commands[i].exec(argc - 2, argv + 2);
			break;
		}

	if(i == chips_num_commands)
		return chips_usage(argv[0]);

	//fs_shutdown(); // this seems to revert our changes, so I'm skipping this.

	potatoes_set_current_disk(NULL);
	potatoes_disc_destroy(disk);

	return ret;
}
