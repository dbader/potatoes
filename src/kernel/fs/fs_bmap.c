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

#include "fs_const.h"
#include "fs_types.h"

#include "fs_super.h"

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
        uint8 bmap[NUM_BLOCKS_ON_HD / 8];
 /********************************************/


/**
 * Resets the block bitmap.
 * Set boot block, super block, ... (until first "real" data block) as used.
 */
void reset_bmap()
{
        struct super_block *super = get_super();
        block_nr first_data_block = super->s_first_data_block;
        
        for (block_nr i = 0; i < first_data_block; i++){
                mark_block(i, TRUE);
        }
        
        for (block_nr i = first_data_block; i < NUM_BLOCKS_ON_HD; i++){
                mark_block(i, FALSE);
        }
}
        
/**
 * Find a new unused block.
 * Search linear from the "start" block number.
 * 
 * @param start Search offset (start to search linear from block number "start")
 * @return The block number of the unused block.
 */
block_nr get_free_block(block_nr start)
{
        block_nr blk = start;
        
        while(blk < NUM_BLOCKS_ON_HD){
               uint8 byte = bmap[blk / 8];
               for (int i = 0; i < 8; i++){
                       byte = byte << i;
                       if ((byte | 0x7F) == FALSE){  //byte OR 01111111 is FALSE -> first bit is 0
                               return blk;
                       } else{
                               blk++; //try next block
                       }
               }
        }
        return NOT_FOUND;
}

/**
 * Function to mark a block number as free (flag = 0) or used (flag = 1)
 * 
 * @param blk_nr The block number which should be marked
 * @param flag   The value with whom the block should be marked (1 | 0)            
 */
void mark_block(block_nr blk_nr, bool flag)
{
        uint8 byte = bmap[blk_nr / 8];
        uint8 bit  = blk_nr % 8;
        
        byte = byte << bit;
        
        if (flag == TRUE){
                byte = byte | 0x80;  //set first bit to 1 with (byte OR 10000000)
        } else {
                byte = byte & 0x7F;  //set first bit to 0 with (byte AND 0111111) 
        }
        
        byte = byte >> bit;
        
        bmap[blk_nr / 8] = byte;
}

/**
 * Allocates a new block.
 * 
 * @param start Search offset (start to search linear from block number "start")
 */

block_nr alloc_block(block_nr start){
        block_nr blk_nr;
        blk_nr = get_free_block(start);
        mark_block(blk_nr, TRUE);
        return blk_nr;
}
