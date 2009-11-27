/* $Id: fs_inode_table.h 266 2009-10-14 08:15:36Z dtraytel $
********************************************************************************
* _____   ____ _______    _______ ____  ______  _____                          *
*|  __ \ / __ \__   __|/\|__   __/ __ \|  ____|/ ____|          Copyright 2008 *
*| |__) | |  | | | |  /  \  | | | |  | | |__  | (___              Daniel Bader *
*|  ___/| |  | | | | / /\ \ | | | |  | |  __|  \___ \           Vincenz Doelle *
*| |    | |__| | | |/ ____ \| | | |__| | |____ ____) |    Johannes Schamburger *
*|_|     \____/  |_/_/    \_\_|  \____/|______|_____/          Dmitriy Traytel *
*                                                                              *
*      Practical Oriented TeAching Tool, Operating (and) Educating System      *
*                                                                              *
*                           www.potatoes-project.tk                            *
*******************************************************************************/

/**
 * @file
 * The inode table.
 * This table contains the currently opened inodes.
 * This table is only existent in memory.
 *
 * @author Vincenz Doelle
 * @author $LastChangedBy: dtraytel $
 * @version $Rev: 266 $
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
