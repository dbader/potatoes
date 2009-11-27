/* $Id: fs_super.h 266 2009-10-14 08:15:36Z dtraytel $
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
 * The superblock table.
 *
 * @author Vincenz Doelle
 * @author $LastChangedBy: dtraytel $
 * @version $Rev: 266 $
 */

#ifndef __SUPER_H_
#define __SUPER_H_

/**
 * extern deklaration of root inode
 */
extern m_inode *root;

struct super_block {
        block_nr s_HD_size;                             /* #blocks on HD */
        uint16  s_bmap_blocks;                          /* # blocks used by block bitmap */
        block_nr s_first_data_block;
        uint32 s_max_file_size;

        uint8 *s_bmap;                                  /* pointer to block bitmap */
        m_inode *s_iroot;                               /* pointer to root inode */
        time_t s_modify_ts;
        bool s_read_only;                               /* TRUE or FALSE */
        uint16 s_dirt;                                  /* CLEAN or DIRTY */
        uint32 s_magic_number;

}__attribute__((packed)) super_block;

void init_super_block();

void load_super_block();

void write_super_block();

void dump_super();

#define NIL_SUPER (struct super_block *) 0

#endif /*SUPER_H_*/
