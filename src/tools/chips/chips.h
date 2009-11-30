#ifndef FS_LIB_H
#define FS_LIB_H

/* Stuff copied from potatoes pending a better solution... */
typedef unsigned char   potatoes_uint8;
typedef signed   char   potatoes_sint8;
typedef unsigned short  potatoes_uint16;
typedef signed   short  potatoes_sint16;
typedef unsigned int    potatoes_uint32;
typedef signed   int    potatoes_sint32;

typedef float   potatpes_float32;
typedef double  potatpes_float64;

typedef potatoes_uint8   potatoes_bool;

typedef potatoes_uint32  potatoes_size_t;
typedef potatoes_uint32  potatoes_block_nr;
typedef potatoes_sint16  potatoes_inode_nr;
typedef potatoes_sint16  potatoes_file_nr;

struct potatoes_time {
        potatoes_uint8 sec;
        potatoes_uint8 min;
        potatoes_uint8 hour;
        potatoes_uint8 weekday;
        potatoes_uint8 day;
        potatoes_uint8 month;
        potatoes_uint8 year;
        potatoes_uint8 century;
};
typedef struct potatoes_time potatoes_time_t;

#define POTATOES_DATA_FILE               1
#define POTATOES_DIRECTORY               2
#define POTATOES_NAME_SIZE               28
#define POTATOES_BLOCK_SIZE              512
#define POTATOES_DIR_ENTRY_SIZE          sizeof (struct potatoes_dir_entry)
#define POTATOES_DIR_ENTRIES_PER_BLOCK   ((POTATOES_BLOCK_SIZE)/(POTATOES_DIR_ENTRY_SIZE))


typedef struct potatoes_dir_entry {
        potatoes_block_nr inode;
        char name[POTATOES_NAME_SIZE];
} __attribute__((packed)) potatoes_dir_entry;

typedef struct potatoes_file_info {
        char name[POTATOES_NAME_SIZE];
        potatoes_uint16 mode;
        potatoes_uint32 size;
        potatoes_time_t create_ts;
        potatoes_time_t modify_ts;
        potatoes_uint16 num_links;
} potatoes_file_info_t;

/* end of copied stuff */

typedef struct
{
	unsigned char *data;
	unsigned int size;
} PotatoesDisk;

PotatoesDisk *potatoes_disc_create(char const *file);
void potatoes_disc_destroy(PotatoesDisk *disk);

void potatoes_set_current_disk(PotatoesDisk *disk);

void fs_init();
void fs_shutdown();

potatoes_file_nr do_open(char *abs_path);
int do_close(potatoes_file_nr fd);

potatoes_size_t do_read(potatoes_file_nr fd, void *buf, potatoes_size_t count, potatoes_uint32 pos);
potatoes_size_t do_write(potatoes_file_nr fd, void *buf, potatoes_size_t count, potatoes_uint32 pos);

potatoes_file_info_t *get_file_info(potatoes_file_nr fd, potatoes_file_info_t* info);

potatoes_bool fs_truncate(char *abs_path, potatoes_uint32 size);
potatoes_bool fs_create(char *abs_path, int data_type);
potatoes_bool fs_delete(char *abs_path);

#endif
