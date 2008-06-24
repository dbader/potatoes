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
#define NAME_SIZE               28  
#define DIR_ENTRY_SIZE          sizeof (struct dir_entry)               //4 + NAME_SIZE
#define DISK_INODE_SIZE         sizeof (struct d_inode)                 //2 + 5*4 + NUM_DIRECT_POINTER*4 
#define MEM_INODE_SIZE          sizeof (struct m_inode)                 //2*2 + 6*4 + NUM_DIRECT_POINTER*4 
#define SUPER_SIZE              sizeof (struct super_block)

#define INODES_PER_BLOCK        1
#define DIR_ENTRIES_PER_BLOCK   ((BLOCK_SIZE)/(DIR_ENTRY_SIZE))

#define NUM_FILES               10                                      /* 64; #entries in filp table */
#define NUM_PROC_FILES          20                                      /* #concurrently opened files per process */
#define NUM_INODES              10                                     /* 200; max. num of opened inodes(files+dir) */

#define ADDR_SIZE               4
#define ADDRS_PER_BLOCK         BLOCK_SIZE/ADDR_SIZE
#define NUM_DIRECT_POINTER      25                                      /* # direct pointer in inode */
#define BYTES_DIRECT            NUM_DIRECT_POINTER * BLOCK_SIZE
#define BYTES_SINGLE_INDIRECT   ADDRS_PER_BLOCK * BLOCK_SIZE
#define BYTES_DOUBLE_INDIRECT   ADDRS_PER_BLOCK * ADDRS_PER_BLOCK * BLOCK_SIZE


#define NUM_BLOCKS_ON_HD        131072                                  /* 131072 * 512 byte = 64 MB */
#define NUM_BMAP_BLOCKS         ((NUM_BLOCKS_ON_HD / BLOCK_SIZE) / 8)   /* #blocks for the block bitmap (rounded)*/

#define BOOT_BLOCK              (block_nr) 0
#define SUPER_BLOCK             (block_nr) 1
#define FIRST_BMAP_BLOCK        (block_nr) 2
#define ROOT_INODE              (inode_nr) 0
#define ROOT_INODE_BLOCK        (block_nr) (SUPER_BLOCK + (NUM_BMAP_BLOCKS))
#define FIRST_DATA_BLOCK        ROOT_INODE_BLOCK + 1

#define DATA_FILE               1
#define DIRECTORY               2

#define CLEAN                   0
#define DIRTY                   1

#define NOT_EXISTENT            -1
#define NOT_FOUND               -1
#define NOT_POSSIBLE            -1

#define FD_OFFSET               10                                      /* offset for file descriptor numbers */

#define MAGIC_NUMBER            280187

#endif /*__FS_CONST_H_*/
