/* $Id: types.h 37 2008-05-02 22:59:03Z vdoelle $
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
 * @author $LastChangedBy: vdoelle $
 * @version $Rev: 37 $
 */

#ifndef TYPES_H_
#define TYPES_H_

typedef uint32 block_nr;
typedef uint16 inode_nr;


typedef struct{
        block_nr inode;
        char name[NAME_SIZE];
}dir_entry;


typedef struct {
        uint16 i_mode;                         /* file, directory */
        uint16 i_ref_count;
        uint32 i_size;
        uint32 i_create_ts;
        uint32 i_modify_ts;
        block_nr i_direct_pointer[20];          /* 20*32Bit = 640Bit */
        block_nr i_single_indirect_pointer;
        block_nr i_double_indirect_pointer;
}d_inode;


typedef struct{
        block_nr i_adr;
        uint16 i_mode;
        uint16 i_ref_count;
        uint32 i_size;
        uint32 i_create_ts;
        uint32 i_modify_ts;
        block_nr i_direct_pointer[20];          /* 20*32Bit = 640Bit */
        block_nr i_single_indirect_pointer;
        block_nr i_double_indirect_pointer;
}m_inode;


typedef struct{
        uint16 f_desc;                          /* file (f) descriptor */
        m_inode *f_inode;
        uint8  f_name[NAME_SIZE];               /* file name */
        uint16 f_mode;                          /* see i_mode */
        uint16 f_count;                         /* number of process file descriptors accessing this file */
}file;


typedef struct{
        uint16 pf_desc;                         /* process file (pf) descriptor */
        uint16 *pf_f_desc;                      /* pointer to global file descriptor */
        uint32 pf_pos;                          /* position in file */
}proc_file;                                     /* process file descriptor from process filp */


/* BITMAPS */

typedef struct{
        uint16 bit : 1;
}bmap;                                          /* block bitmap */


/* BUFFER TYPES */

typedef struct{
        uint32 block_nr;
        uint8 cache[BLOCK_SIZE];                /* 32*32 = 1024 bit space for block content */
}block_buf;

typedef block_buf block_cache;

typedef block_buf write_cache;

typedef block_buf crt_ind_adr_block;            /* current indirect address block */



#endif /*TYPES_H_*/
