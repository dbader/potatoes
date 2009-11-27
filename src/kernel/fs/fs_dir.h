/* $Id: fs_dir.h 266 2009-10-14 08:15:36Z dtraytel $
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
 * Basic functions concerning directories.
 *
 * @author Vincenz Doelle
 * @author $LastChangedBy: dtraytel $
 * @version $Rev: 266 $
 */
#ifndef FS_DIR_H_
#define FS_DIR_H_

block_nr find_filename(dir_entry file_list[DIR_ENTRIES_PER_BLOCK], char *name);
bool contains_filename(dir_entry file_list[DIR_ENTRIES_PER_BLOCK], char *name);

block_nr search_file(char *path);
block_nr rfsearch(block_nr crt_dir, char *path, char *tok, char delim[]);

block_nr insert_file_into_dir(block_nr dir_inode_blk, char *name);
uint32 create_entry(dir_entry file_list[DIR_ENTRIES_PER_BLOCK], block_nr blk_nr, char *name);

block_nr delete_file_from_dir(block_nr dir_inode_blk, char *name);
uint32 delete_entry(dir_entry file_list[DIR_ENTRIES_PER_BLOCK], char *name);

char* get_filename(char *abs_path);
char* get_path(char *abs_path);

#endif /*FS_DIR_H_*/
