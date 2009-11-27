/* $Id: fs_main.h 266 2009-10-14 08:15:36Z dtraytel $
********************************************************************************
* _____   ____ _______    _______ ____  ______  _____                          *
*|  __ \ / __ \__   __|/\|__   __/ __ \|  ____|/ ____|          Copyright 2008 *
*| |__) | |  | | | |  /  \  | | | |  | | |__  | (___              Daniel Bader *
*|  ___/| |  | | | | / /\ \ | | | |  | |  __|  \___ \           Vincenz Doelle *
*| |    | |__| | | |/ ____ \| | | |__| | |____ ____) |    Johannes Schamburger *
*|_|     \____/  |_/_/    \_\_|  \____/|______|_____/          Dmitriy Traytel *
*                                                                              *
*      Practical Oriented TeAching Tool, Operating (and) Educating System      *
*                                                                              *
*                           www.potatoes-project.tk                            *
*******************************************************************************/

/**
 * @file
 * Basic definitions of all functions concerning work on files
 *
 * @author Vincenz Doelle
 * @author $LastChangedBy: dtraytel $
 * @version $Rev: 266 $
 */

#ifndef __FS_MAIN_H_
#define __FS_MAIN_H_


/* MKFS */
void fs_init();
void fs_shutdown();

bool load_fs();
bool create_fs();


/* CREATE */
bool do_create(char *abs_path, uint8 mode);
bool do_mkdir(char *abs_path);
bool do_mkfile(char *abs_path);

/* DELETE */
int do_remove(char *abs_path);

/* OPEN */
file_nr do_open(char *abs_path);

/* CLOSE */
int do_close(file_nr fd);
int do_close_pf(proc_file pft[NUM_PROC_FILES], file_nr pfd);

/* READ */
size_t do_read(file_nr fd, void *buf, size_t count, uint32 pos);

/* WRITE */
size_t do_write(file_nr fd, void *buf, size_t count, uint32 pos);

/* LSEEK */
size_t do_lseek(proc_file pft[NUM_PROC_FILES], file_nr fd, sint32 offset, uint32 origin);

/* FILE INFO */
file_info_t* get_file_info(file_nr fd, file_info_t* info);

/* FILE EXISTS */
bool do_file_exists(char *path);

/* DEBUG */
void dump_consts();

#endif /*__FS_MAIN_*/
