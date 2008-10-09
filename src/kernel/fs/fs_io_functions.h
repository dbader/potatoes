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

size_t lseek(proc_file pft[NUM_PROC_FILES], file_nr fd, sint32 offset, uint32 originf);


extern void read_dinode(d_inode *inode, block_nr inode_blk);

extern void read_minode(m_inode *inode, block_nr inode_blk);

extern void write_inode(m_inode *inode);


#endif /*FS_IO_FUNCTIONS_H_*/
