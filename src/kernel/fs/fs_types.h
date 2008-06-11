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
 * Basic type definitions. This file defines basic data types used
 * throughout the file system. You should use these types whenever possible
 * in order to avoid the possible ambiguity of the builtin types.
 *
 * @author Vincenz Doelle
 * @author $LastChangedBy$
 * @version $Rev$
 */

#ifndef __FS_TYPES_H_
#define __FS_TYPES_H_

typedef uint32 block_nr;
typedef sint16 inode_nr;
typedef sint16 file_nr;

/**
 * The directory entry.
 */
typedef struct dir_entry{
        block_nr inode;
        char name[NAME_SIZE];
} dir_entry;


/**
 * The inode on disk.
 */
typedef struct d_inode{
        uint16 i_mode;                          /* file | directory */
        uint32 i_size;                          /* in byte */
        uint32 i_create_ts;
        uint32 i_modify_ts;
        block_nr i_direct_pointer[NUM_DIRECT_POINTER];          /* 25*32Bit = 800Bit*/
        block_nr i_single_indirect_pointer;
        block_nr i_double_indirect_pointer;
} d_inode;


/**
 * The inode in memory.
 */
typedef struct m_inode{
        inode_nr i_num;
        block_nr i_adr;
        uint16 i_mode;
        uint32 i_size;
        uint32 i_create_ts;
        uint32 i_modify_ts;
        block_nr i_direct_pointer[NUM_DIRECT_POINTER];          /* 25*32Bit = 800Bit */
        block_nr i_single_indirect_pointer;
        block_nr i_double_indirect_pointer;
} m_inode;


/**
 * The global file descriptor.
 */
typedef struct file{
        file_nr f_desc;                         /* file (f) descriptor */
        m_inode *f_inode;
        char *f_name;                           /* pointer to absolute file path */
        uint8 f_mode;                           /* see i_mode */
        uint16 f_count;                         /* number of process file descriptors accessing this file */
} file;

/**
 * The process file descriptor.
 */
typedef struct proc_file{
        file_nr pf_desc;                        /* process file (pf) descriptor */
        file_nr pf_f_desc;                      /* pointer to global file descriptor */
        uint32 pf_pos;                          /* position in file */
} proc_file;                                    /* process file descriptor from process filp */


/**
 * A buffer for one block.
 */
typedef struct {
        block_nr block_nr;
        uint8 cache[BLOCK_SIZE];                /* BLOCK_SIZE * 1 byte = BLOCK_SIZE byte space for block content */
} block_buffer;


/**
 * The cache for one generic block.
 */  
typedef block_buffer block_cache;



#endif /*__FS_TYPES_H_*/
