/* $Id$
      _   _  ____   _____
     | | (_)/ __ \ / ____|
  ___| |_ _| |  | | (___
 / _ \ __| | |  | |\___ \  Copyright 2008 Daniel Bader, Vincenz Doelle,
|  __/ |_| | |__| |____) |        Johannes Schamburger, Dmitriy Traytel
 \___|\__|_|\____/|_____/

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file
 * Basic functions concerning directories.
 *
 * @author Vincenz Doelle
 * @author $LastChangedBy$
 * @version $Rev$
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
