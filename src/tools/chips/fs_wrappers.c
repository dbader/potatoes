#include "chips.h"

#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void potatoes_bzero(void *dest, potatoes_uint32 count)
{
	bzero(dest, count);
}

void *potatoes_malloc(potatoes_uint32 size)
{
	return malloc(size);
}

void *potatoes_mallocn(potatoes_uint32 size, char *name)
{
	return malloc(size); // I don't care about the name...
}

void potatoes_free(void *start)
{
	free(start);
}

void potatoes_mem_dump()
{
	fprintf(stderr, "NOT IMPLEMENTED: potatoes_mem_dump\n");
}

char *potatoes_strncpy(char *dest, char *src, size_t n)
{
	return strncpy(dest, src, n);
}

potatoes_uint32 potatoes_strlen(char* str)
{
	return strlen(str);
}

char *potatoes_strdup(char* str)
{
	return strdup(str);
}

char *potatoes_strsep(char **str_ptr, char *delims)
{
	return strsep(str_ptr, delims);
}

potatoes_sint32 potatoes_strcmp(char *s1, char *s2)
{
	return strcmp(s1, s2);
}

void *potatoes_memcpy(void *dest, void *src, potatoes_size_t count)
{
	return memcpy(dest, src, count);
}

void potatoes_printf(char *fmt, ...)
{
	//FILE *logfile;
	//logfile = fopen("log", "a");
	va_list va;
	va_start(va, fmt);
	//vfprintf(logfile, fmt, va);
	vfprintf(stderr, fmt, va);
	va_end(va);
	//fclose(logfile);
}

char *potatoes_strncat(char *s1, char *s2, potatoes_size_t n)
{
	return strncat(s1, s2, n);
}

char *potatoes_strcat(char *s1, char *s2)
{
	return strcat(s1, s2);
}

void potatoes_panic(char *msg)
{
	fprintf(stderr, "PANIC: %s (I'll refuse to crash your os...)\n", msg);
	exit(1);
}

char *potatoes_itoa(int n, char *str, unsigned int base)
{
	if(base != 10)
	{
		fprintf(stderr, "WARNING: base is currently ignored in itoa...\n");
	}
	sprintf(str, "%i", n);
	return str;
}

char *potatoes_time2str(potatoes_time_t timestamp, char buf[24])
{
	strcpy(buf, "time2str stub.");
	return buf;
}

PotatoesDisk *potatoes_current_disc = NULL;

potatoes_uint32 potatoes_get_hdsize()
{
	if(potatoes_current_disc == NULL)
	{
		fprintf(stderr, "ERROR: no disc selected in get_hdsize\n");
		return 0;
	}
	return potatoes_current_disc->size;
}

void potatoes_hd_write_sector(potatoes_uint32 dest, void* src)
{
	if(potatoes_current_disc == NULL)
	{
		fprintf(stderr, "ERROR: no disc selected in hd_write_sector\n");
		return;
	}

	if((dest + 1) * 512 > potatoes_current_disc->size)
	{
		fprintf(stderr, "ERROR: no sector %i in hd_write_sector\n", dest);
		return;
	}

	memcpy(potatoes_current_disc->data + (dest * 512), src, 512);
}

void potatoes_hd_read_sector(void* dest, potatoes_uint32 src)
{
	if(potatoes_current_disc == NULL)
	{
		fprintf(stderr, "ERROR: no disc selected in hd_read_sector\n");
		return;
	}

	if((src + 1) * 512 > potatoes_current_disc->size)
	{
		fprintf(stderr, "ERROR: no sector %i in hd_read_sector\n", src);
		return;
	}

	memcpy(dest, potatoes_current_disc->data + (src * 512), 512);
}

PotatoesDisk *potatoes_disc_create(char const *file)
{
	PotatoesDisk *disk = malloc(sizeof(PotatoesDisk));
	if(disk == NULL)
		return NULL;

	int fd = open(file, O_RDWR);
	if(fd < 0)
	{
		fprintf(stderr, "ERROR: failed to open %s in potatoes_disc_create\n", file);
		free(disk);
		return NULL;
	}

	struct stat mystat;
	if(fstat(fd, &mystat) < 0)
	{
		fprintf(stderr, "ERROR: failed to stat %s in potatoes_disc_create\n", file);
		free(disk);
		close(fd);
		return NULL;
	}

	disk->size = mystat.st_size;

	if((disk->data = (unsigned char *)mmap(NULL, mystat.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0)) == NULL)
	{
		fprintf(stderr, "ERROR: failed to mmap %s in potatoes_disc_create\n", file);
		free(disk);
		close(fd);
		return NULL;
	}

	close(fd);
	return disk;
}

void potatoes_disc_destroy(PotatoesDisk *disk)
{
	if(disk == NULL)
		return;

	munmap(disk->data, disk->size);
	free(disk);
}

void potatoes_set_current_disk(PotatoesDisk *disk)
{
	potatoes_current_disc = disk;
}
