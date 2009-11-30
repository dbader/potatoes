/* $Id$
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
 * Functions definitions concerning read, write, open, close, create, delete.
 *
 * @author Vincenz Doelle
 * @author $LastChangedBy$
 * @version $Rev$
 */

#ifndef FS_IO_FUNCTIONS_H_
#define FS_IO_FUNCTIONS_H_

#define CREATE  1
#define DELETE  2


size_t fs_read(void *buf, m_inode *inode, size_t num_bytes, uint32 pos, bool allow_enlargement);

size_t fs_write(m_inode *inode, void *buf, size_t num_bytes, uint32 pos, bool allow_enlargement);

file_nr fs_open(char *abs_path);

bool fs_close(file_nr fd);

bool fs_create(char *path, int data_type);

bool fs_delete(char *path);

bool fs_create_delete(char *abs_path, int mode, int data_type);

bool fs_truncate(char *abs_path, uint32 size);

size_t lseek(proc_file pft[NUM_PROC_FILES], file_nr fd, sint32 offset, uint32 originf);


extern void read_dinode(d_inode *inode, block_nr inode_blk);

extern void read_minode(m_inode *inode, block_nr inode_blk);

extern void write_inode(m_inode *inode);

#endif /*FS_IO_FUNCTIONS_H_*/
