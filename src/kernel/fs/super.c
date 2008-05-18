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

#include "const.h"
#include "types.h"
#include "super.h"

/**
 * Pointer to the root inode.
 */       
extern m_inode *root;

/**
 * The bitmap for the HD blocks.
 */
extern bit bmap[NUM_BLOCKS_ON_HD];

extern void reset_bmap();

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
        
        super->s_first_data_block = 1 /*boot*/ + 1 /*super*/ + NUM_BMAP_BLOCKS /*block bitmap*/;
        
        /* with 32 Bit addresses and a block size of 1024: 65 MB */
        super->s_max_file_size = (NUM_DIRECT_POINTER + BLOCK_SIZE*8/32 + (BLOCK_SIZE*8/32)*(BLOCK_SIZE*8/32)) * BLOCK_SIZE;
        
        /* pointer to blocks of block bitmap */
        super->s_bmap = &bmap[0];
        
        /* pointer to root inode */
        //super->s_iroot = root;
        
        //super->s_modify_ts = get_time();
        
        super->s_read_only = FALSE;
        
        super->s_dirt = FALSE;
        
}


/**
 * Load the super block from HD.
 */
void load_super_block()
{
        //TODO
}

/**
 * @return pointer to super block structure.
 */
struct super_block* get_super()
{
        return super;
}

