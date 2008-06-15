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
 * The inode table.
 * This table contains the currently opened inodes.
 * This table is only existent in memory.
 *
 * @author Vincenz Doelle
 * @author $LastChangedBy$
 * @version $Rev$
 */

#ifndef __FS_INODE_TABLE_H_
#define __FS_INODE_TABLE_H_

/** The (memory-) inode table */
m_inode inode_table[NUM_INODES];

/** The root inode */
m_inode *root;


/* definitions */

#define NIL_INODE -1   


/* functions */

void init_inode_table();

void dump_inode(m_inode *i);

void dump_inodes();

void load_root();

void write_root();

void create_root();

m_inode* get_inode(inode_nr i_num);

void read_dinode(d_inode *inode, block_nr inode_blk);

void read_minode(m_inode *inode, block_nr inode_blk);

void write_inode(m_inode *inode);

void write_inodes();

void cpy_minode_to_dinode(d_inode *di, m_inode *mi);

void cpy_dinode_to_minode(m_inode *mi, d_inode *di);

m_inode* new_minode(block_nr adr, int mode, bool to_inode_table);

void free_inode(inode_nr i_num);

m_inode* alloc_inode();


#endif /*__FS_INODE_TABLE_H_*/
