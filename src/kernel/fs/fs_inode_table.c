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

#include "../include/const.h"
#include "../include/types.h"
#include "../include/string.h"

#include "fs_const.h"
#include "fs_types.h"

#include "fs_buf.h"
#include "fs_block_dev.h"
#include "fs_inode_table.h"

/**
 * Initialize all inodes with i_num = NIL_INODE = 0
 * -> sign that inode is unused
 */
void init_inode_table()
{
        for (int i = 0; i < NUM_INODES; i++){
                inode_table[i].i_num = NIL_INODE;
        }
        
        //load root inode from HD to inode table
        rd_block(&inode_table[0], ROOT_INODE_BLOCK, BLOCK_SIZE);
        inode_table[0].i_num = ROOT_INODE;
 
        root = &inode_table[0];
}

/**
 * Returns a pointer to an inode with inode_nr = i_num.
 * 
 * @param i_num The inode number of the inode
 * @return Pointer to the inode.
 */
m_inode* get_inode(inode_nr i_num)
{
        for (int i = 0; i < NUM_INODES; i++){
                if (inode_table[i].i_num == i_num){
                        return &inode_table[i];
                }
        }
        return (m_inode*) 0;
}

/**
 * Write a memory inode to disk.
 * First transform memory inode to a disk inode. Then copy this disk inode to the write_cache.
 * Finally write the write_cache to disk.
 * 
 * @param inode Pointer to the memory inode which should be written
 * @return boolean status of operation
 */
//TODO: counter-check this after implementation of the buffers!
bool write_inode(m_inode *inode)
{
        clear_cache(&write_cache);                         //reset write cache
        
        cpy_minode_to_dinode(&d_inode_cache, inode);
        
        memcpy(write_cache.cache, &d_inode_cache, sizeof(d_inode));
                
        wrt_cache(&write_cache, BLOCK_SIZE);
        
        return TRUE;
}        

/**
 * Copy common content of a memory inode to a disk inode.
 * 
 * @param mi The memory inode
 * @param di The disk inode
 */
void cpy_minode_to_dinode(d_inode *di, m_inode *mi)
{
        di->i_mode      = mi->i_mode;
        di->i_size      = mi->i_size;
        di->i_create_ts = mi->i_create_ts;
        di->i_modify_ts = mi->i_modify_ts;
        
        for (int i = 0; i < NUM_DIRECT_POINTER; i++){
                di->i_direct_pointer[i] = mi->i_direct_pointer[i];
        }
        
        di->i_single_indirect_pointer = mi->i_single_indirect_pointer;
        di->i_double_indirect_pointer = mi->i_double_indirect_pointer;
}

void cpy_dinode_to_minode(m_inode *mi, d_inode *di)
{
        mi->i_mode                      = di->i_mode;
        mi->i_size                      = di->i_size;
        mi->i_create_ts                 = di->i_create_ts;
        mi->i_modify_ts                 = di->i_modify_ts;
        
        for (int i = 0; i < NUM_DIRECT_POINTER; i++){
                mi->i_direct_pointer[i] = di->i_direct_pointer[i];
        }

        mi->i_single_indirect_pointer   = di->i_single_indirect_pointer;
        mi->i_double_indirect_pointer   = di->i_double_indirect_pointer;
}

/**
 * Sets an inode as unused.
 * 
 * @param i_num The inode number
 */
void free_inode(inode_nr i_num)
{
      m_inode *i = get_inode(i_num);
      i->i_num = NIL_INODE;             //overwrite inode number with -1 -> "unused"
}

/**
 * Returns an unused inode.
 * 
 * @return Pointer to the inode
 */
m_inode* alloc_inode(void)
{
        for (int i = 0; i < NUM_INODES; i++){
                if (inode_table[i].i_num == NIL_INODE){
                        inode_table[i].i_num = i;
                        return &inode_table[i];
                }
        }
        return (m_inode*) 0;
}
