/* $Id$
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
 * Basic functions of the super block.
 *
 * @author Vincenz Doelle
 * @author $LastChangedBy$
 * @version $Rev$
 */

#include "../include/const.h"
#include "../include/types.h"

#include "../include/debug.h"
#include "../include/assert.h"

#include "fs_const.h"
#include "fs_types.h"
#include "fs_super.h"
#include "fs_block_dev.h"
#include "fs_bmap.h"
#include "../io/io_rtc.h"

/*
 * @see /kernel/io/io_harddisk.h
 */
extern uint32 get_hdsize();

/**
 * Pointer to super block structure.
 */
struct super_block *super;

/**
 * Initializes the super block.
 */
void init_super_block()
{
        super = &super_block;

        super->s_HD_size = get_hdsize();

        super->s_bmap_blocks = num_bmap_blocks;

        super->s_first_data_block = first_data_block;

        /* with 32 Bit addresses and a block size of 512: 16MB */
        super->s_max_file_size = (NUM_DIRECT_POINTER + BLOCK_SIZE * 8 / 32 + (BLOCK_SIZE * 8 / 32) * (BLOCK_SIZE * 8 / 32)) * BLOCK_SIZE;

        /* pointer to blocks of block bitmap */
        super->s_bmap = bmap;

        /* pointer to root inode */
        super->s_iroot = root;

        super->s_modify_ts = time;

        super->s_read_only = FALSE;

        super->s_dirt = FALSE;

        super->s_magic_number = MAGIC_NUMBER; //assume that we use the FS now

}

/**
 * Dump the main attributes.
 */
void dump_super()
{
        ASSERT(super != NIL_SUPER);
        fs_dprintf("\nSUPER: \n");
        fs_dprintf("HD_size = %d\n#bmap_blocks = %d\nFDB = %d\nMFS = %d\n*bmap = 0x%p\n*iroot = 0x%p\nmagic = %d\n\n",
                   super->s_HD_size, super->s_bmap_blocks, super->s_first_data_block,
                   super->s_max_file_size, super->s_bmap, super->s_iroot, super->s_magic_number);
}


/**
 * Load the super block from HD.
 */
void load_super_block()
{
        fs_dprintf("[fs_super] read block number SUPER_BLOCK (1) to superblock struct\n");
        //read block number SUPER_BLOCK (1) to superblock struct
        rd_block(&super_block, SUPER_BLOCK, sizeof(super_block));
        super = &super_block;

        ASSERT(super != NIL_SUPER);
}

/**
 * Write the super block from memory to HD.
 */
void write_super_block()
{
        wrt_block(SUPER_BLOCK, super, sizeof(super_block));
}
