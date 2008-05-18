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
 * Functions to manage the block bitmap (bmap)
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
 * Functions on the block bitmap
 */

void reset_bmap();

block_nr get_free_block(block_nr start);

void mark_block(block_nr blk, bool flag);



/**
 * Central block bitmap structure.
 */
 /********************************************/
        bit bmap[NUM_BLOCKS_ON_HD];
        //uint8 bmap[NUM_BLOCKS_ON_HD / 8];
 /********************************************/


/**
 * Resets the block bitmap
 */
void reset_bmap()
{
        struct super_block *super = get_super();
        block_nr first_data_block = super->s_first_data_block;
        
        block_nr i;
        for (i = 0; i < first_data_block; i++)
                bmap[i].bit = 1;
        
        for (i = first_data_block; i < NUM_BLOCKS_ON_HD; i++)
                bmap[i].bit = 0;
}
        
/**
 * Find a new unused block.
 * Search linear from the "start" block number.
 */
block_nr get_free_block(block_nr start)
{
        block_nr blk = start;
        
        while(blk++ < NUM_BLOCKS_ON_HD){
                if (bmap[blk].bit == 0)
                        return blk;
        }
        return NOT_FOUND;
}

/**
 * Function to mark a block number as free (flag = 0) or used (flag = 1)
 */
void mark_block(block_nr blk_nr, bool flag)
{
        bmap[blk_nr].bit = flag;
        
        /* uint8 byte = bmap[19];
         * zu finden: bit 4 in byte 19...
         * 
         * byte = byte << 3 ;//rotate
         * byte = byte & 0x10;
         * 
         * if (byte != 0) -> set...
         */  
}
