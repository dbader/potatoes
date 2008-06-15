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
#include "../include/stdlib.h"

#include "../include/debug.h"
#include "../include/assert.h"

#include "fs_const.h"
#include "fs_types.h"

#include "fs_buf.h"
#include "fs_block_dev.h"
#include "fs_inode_table.h"

/**
 * Initialize all inodes with i_num = NIL_INODE = -1
 * -> sign that inode is unused
 */
void init_inode_table()
{
        dprintf("resetting inode_table...");
        for (int i = 0; i < NUM_INODES; i++){
                inode_table[i].i_num = NIL_INODE;
        }
        dprintf("done\n");
}


void dump_inode(m_inode *mi)
{
        dprintf("INODE: num = %d; adr = %d; sip = %d; dip = %d; mode = %d\n", 
                 mi->i_num, mi->i_adr,
                 mi->i_single_indirect_pointer,
                 mi->i_double_indirect_pointer,
                 mi->i_mode);
        for (int i = 0; i < NUM_DIRECT_POINTER; i++){
                dprintf("dp[%d] = %d; ", i, mi->i_direct_pointer[i]);
        }
        dprintf("\n");
}

void dump_dinode(d_inode *mi)
{
        dprintf("INODE: sip = %d; dip = %d; mode = %d\n", 
                 mi->i_single_indirect_pointer,
                 mi->i_double_indirect_pointer,
                 mi->i_mode);
        for (int i = 0; i < NUM_DIRECT_POINTER; i++){
                dprintf("dp[%d] = %d; ", i, mi->i_direct_pointer[i]);
        }
        dprintf("\n");
}

void dump_inodes()
{
        for (int i = 0; i < NUM_INODES; i++){
                dump_inode(&inode_table[i]);
        }
}

void load_root()
{
        dprintf("load root inode from HD to inode table\n");
        //load root inode from HD to inode table
        rd_block(&inode_table[ROOT_INODE], ROOT_INODE_BLOCK, sizeof(m_inode));
        
        root = &inode_table[ROOT_INODE];
        
        ASSERT(root != (m_inode*)0);
        
}

void write_root()
{
        dprintf("write root inode to HD\n");
        //write root inode to HD
        wrt_block(ROOT_INODE_BLOCK, &inode_table[ROOT_INODE], sizeof(m_inode));
}

void create_root()
{
        dprintf("create new root inode\n");
        m_inode *new_root = new_minode(ROOT_INODE_BLOCK, DIRECTORY, FALSE);
        memcpy(&inode_table[ROOT_INODE], new_root, sizeof(m_inode));
        
        root = &inode_table[ROOT_INODE];
        root->i_num = ROOT_INODE;
        
        ASSERT(inode_table[ROOT_INODE].i_num != NIL_INODE);
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
void read_dinode(d_inode *inode, block_nr inode_blk)
{
        rd_block(inode, inode_blk, sizeof(d_inode));
}

void read_minode(m_inode *inode, block_nr inode_blk)
{
        bzero(&d_inode_cache, sizeof(d_inode_cache));
        
        read_dinode(&d_inode_cache, inode_blk);
        cpy_dinode_to_minode(inode, &d_inode_cache);
        inode->i_adr = inode_blk;
}

/**
 * Write a memory inode to disk.
 * First transform memory inode to a disk inode. Then copy this disk inode to the write_cache.
 * Finally write the write_cache to disk.
 * 
 * @param inode Pointer to the memory inode which should be written
 * @return boolean status of operation
 */
void write_inode(m_inode *inode)
{
        cpy_minode_to_dinode(&d_inode_cache, inode);
        
        wrt_block(inode->i_adr, &d_inode_cache, sizeof(d_inode));
}

void write_inodes()
{
        //close all left inodes
        for (int i = 0; i < NUM_INODES; i++){
                write_inode(&inode_table[i]);
        }
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

m_inode* new_minode(block_nr adr, int mode, bool to_inode_table)
{
        m_inode *mi;
        
        if (!to_inode_table){
                mi = malloc(sizeof(m_inode));
                mi->i_num  = 0;
        } else {
                mi = alloc_inode();
        }
        
        mi->i_adr  = adr;
        mi->i_mode = mode;
        mi->i_size = 0;
        mi->i_create_ts = NULL; //TODO: get create_ts from Dmitriy
        
        for (int i = 0; i < NUM_DIRECT_POINTER; i++){
                mi->i_direct_pointer[i] = NULL;
        }
        
        mi->i_single_indirect_pointer = NULL;
        mi->i_double_indirect_pointer = NULL;
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
