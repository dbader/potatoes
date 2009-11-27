/* $Id: fs_file_table.h 266 2009-10-14 08:15:36Z dtraytel $
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
 * The file descriptor table.
 *
 * @author Vincenz Doelle
 * @author $LastChangedBy: dtraytel $
 * @version $Rev: 266 $
 */

#ifndef __FS_FILE_TABLE_H_
#define __FS_FILE_TABLE_H_

/* the global file table (gft) */
file gft[NUM_FILES];


/* functions */

void init_file_table();
void init_proc_file_table(proc_file proc_filp[]);

void dump_file(file *f);
void dump_files();
void dump_proc_file(proc_file *pf);
void dump_proc_files();

file_nr insert_file(m_inode *inode, char *name, uint8 mode);
file_nr insert_proc_file(proc_file pft[NUM_PROC_FILES], file_nr glo_fd);

file* alloc_file();
proc_file* alloc_proc_file(proc_file pft[NUM_PROC_FILES]);

file* get_file(file_nr fd);
proc_file* get_proc_file(proc_file pft[NUM_PROC_FILES], file_nr fd);

void free_file(file_nr fd);
void free_proc_file(proc_file pft[NUM_PROC_FILES], file_nr fd);

bool contains_file(file_nr fd);

file_nr name2desc(char *name);
file_nr inode2desc(m_inode *inode);

void inc_count(file_nr fd);
size_t lseek(proc_file pft[NUM_PROC_FILES], file_nr fd, sint32 offset, uint32 origin);


/* definition */

#define NIL_FILE -1

#define NIL_PROC_FILE -1


#endif /*__FS_FILE_TABLE_H_*/
