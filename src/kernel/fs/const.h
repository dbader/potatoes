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
 * Basic constant definitions. This file defines constants used
 * throughout the file system.
 *
 * @author Vincenz Doelle
 * @author $LastChangedBy$
 * @version $Rev$
 */


#ifndef CONST_H_
#define CONST_H_

// svn:keywords "Id Revision Author"

#define BLOCK_SIZE              1024
#define DIR_ENTRY_SIZE          sizeof (struct dir_entry)
#define DISK_INODE_SIZE         sizeof (struct d_inode)
#define MEM_INODE_SIZE          sizeof (struct m_inode)
#define SUPER_SIZE              sizeof (struct super_block)

#define NAME_SIZE               30

#define INODES_PER_BLOCK        ((BLOCK_SIZE)/(DISK_INODE_SIZE))
#define DIR_ENTRIES_PER_BLOCK   ((BLOCK_SIZE)/(DIR_ENTRY_SIZE)))

#define NUM_FILES               64                                      /*#entries in filp table*/
#define NUM_SUPERS              1                                       /*1 FS -> 1 super block*/
#define NUM_OPEN_FILES          20                                      /*per process*/
#define NUM_INODES              100                                     /*max. num of opened inodes/files*/

extern int get_HD_size();
int *hd_size;
enum { NUM_BLOCKS_ON_HD =       *hd_size};
enum { NUM_BMAP_BLOCKS =        (NUM_BLOCKS_ON_HD)/(BLOCK_SIZE) };            /*#blocks for the block bitmap*/

#define BOOT_BLOCK              (block_nr) 0
#define SUPER_BLOCK             (block_nr) 1
#define ROOT_INODE              (inode_nr) 1

#define CLEAN                   0
#define DIRTY                   1



#endif /*CONST_H_*/
