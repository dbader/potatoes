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
 * The superblock table.
 *
 * @author Vincenz Doelle
 * @author $LastChangedBy$
 * @version $Rev$
 */

#ifndef __SUPER_H_
#define __SUPER_H_

/**
 * extern deklaration of the block bitmap
 */
extern uint8 bmap[NUM_BMAP_BLOCKS * BLOCK_SIZE];

/**
 * extern deklaration of root inode
 */
extern m_inode *root;

struct super_block {
        block_nr s_HD_size;                             /* #blocks on HD */
        uint16  s_bmap_blocks;                          /* # blocks used by block bitmap */
        block_nr s_first_data_block;
        uint32 s_max_file_size;
        
        /* in mem */
        uint8 *s_bmap;                                  /* pointer to block bitmap */
        m_inode *s_iroot;                               /* pointer to root inode */
        uint32 s_modify_ts;
        uint16 s_read_only;                             /* TRUE or FALSE */
        uint16 s_dirt;                                  /* CLEAN or DIRTY */
        bool s_used;          
        
} super_block;

void init_super_block();

void load_super_block();

void write_super_block();

void dump_super();

#define NIL_SUPER (struct super_block *) 0

#endif /*SUPER_H_*/
