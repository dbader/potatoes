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

/**
 * Time function.
 * @return current system time.
 */
extern uint32 get_time();

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
        
        super->s_HD_size = NUM_BLOCKS_ON_HD;
        
        super->s_bmap_blocks = NUM_BMAP_BLOCKS;
        
        super->s_first_data_block = FIRST_DATA_BLOCK;
        
        /* with 32 Bit addresses and a block size of 512: 16MB */
        super->s_max_file_size = (NUM_DIRECT_POINTER + BLOCK_SIZE*8/32 + (BLOCK_SIZE*8/32)*(BLOCK_SIZE*8/32)) * BLOCK_SIZE;
        
        /* pointer to blocks of block bitmap */
        super->s_bmap = bmap;
        
        /* pointer to root inode */
        super->s_iroot = root;
        
        //super->s_modify_ts = get_time();
        
        super->s_read_only = FALSE;
        
        super->s_dirt = FALSE;
        
        super->s_used = TRUE; //assume that we use the FS now
        
}

void dump_super()
{
        ASSERT(super != NIL_SUPER);
        dprintf("\ndumping super: \n");
        dprintf("HD_size = %d\n#bmap_block = %d\nFDB = %d\nMFS = %d\n*bmap = %p\n*iroot = %p\nused = %d\n\n",
                        super->s_HD_size, super->s_bmap_blocks, super->s_first_data_block, 
                        super->s_max_file_size, super->s_bmap, super->s_iroot, super->s_used);
}


/**
 * Load the super block from HD.
 */
void load_super_block()
{
        dprintf("read block number SUPER_BLOCK (1) to superblock struct");
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
