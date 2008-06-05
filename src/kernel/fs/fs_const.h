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


#ifndef __FS_CONST_H_
#define __FS_CONST_H_


#define BLOCK_SIZE              512
#define DIR_ENTRY_SIZE          BLOCK_SIZE //sizeof (struct dir_entry)
#define DISK_INODE_SIZE         BLOCK_SIZE //sizeof (struct d_inode)
#define MEM_INODE_SIZE          BLOCK_SIZE //sizeof (struct m_inode)
#define SUPER_SIZE              BLOCK_SIZE //sizeof (struct super_block)

#define NAME_SIZE               28                                      

#define INODES_PER_BLOCK        ((BLOCK_SIZE)/(DISK_INODE_SIZE))
#define DIR_ENTRIES_PER_BLOCK   ((BLOCK_SIZE)/(DIR_ENTRY_SIZE))

#define NUM_FILES               64                                      /* #entries in filp table */
#define NUM_PROC_FILES          20                                      /* #concurrently opened files per process */
#define NUM_INODES              200                                     /* max. num of opened inodes(files+dir) */

#define ADDR_SIZE               4
#define ADDRS_PER_BLOCK         BLOCK_SIZE/ADDR_SIZE
#define NUM_DIRECT_POINTER      25                                      /* # direct pointer in inode */
#define BYTES_DIRECT            NUM_DIRECT_POINTER * BLOCK_SIZE
#define BYTES_SINGLE_INDIRECT   ADDRS_PER_BLOCK * BLOCK_SIZE
#define BYTES_DOUBLE_INDIRECT   ADDRS_PER_BLOCK * ADDRS_PER_BLOCK * BLOCK_SIZE


#define NUM_BLOCKS_ON_HD        131072                                  /* 131072 * 512 byte = 64 MB */
#define NUM_BMAP_BLOCKS         (NUM_BLOCKS_ON_HD)/(BLOCK_SIZE*8)       /* #blocks for the block bitmap*/

#define BOOT_BLOCK              (block_nr) 0
#define SUPER_BLOCK             (block_nr) 1
#define ROOT_INODE              (inode_nr) 1
#define ROOT_INODE_BLOCK        (block_nr) (BOOT_BLOCK + SUPER_BLOCK + (NUM_BMAP_BLOCKS + 1) /*(rounded)*/ + ROOT_INODE)

#define CLEAN                   0
#define DIRTY                   1

#define NOT_EXISTENT            -1
#define NOT_FOUND               -1
#define NOT_POSSIBLE            -1

#define FD_OFFSET               10                                      /* offset for file descriptor numbers */

#endif /*__FS_CONST_H_*/
