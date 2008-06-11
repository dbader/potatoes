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
#include "../include/debug.h"
#include "../include/string.h"

#include "fs_const.h"
#include "fs_types.h"

#include "fs_super.h"
#include "fs_bmap.h"
#include "fs_block_dev.h"


/**
 * Resets the block bitmap.
 * Set boot block, super block, ... (until first "real" data block) as used.
 */
void reset_bmap()
{
        dprintf("resetting bmap (size = %d)\n", sizeof(bmap));
        
        bzero(bmap, sizeof(bmap));
        
        block_nr fdp = FIRST_DATA_BLOCK;
        
        for (block_nr i = 0; i < fdp; i++){
                mark_block(i, TRUE);
        }
        
        dprintf("bmap reset\n");
}

/**
 * Loads the block bitmap from HD
 */
void load_bmap()
{
        int j = 0;
        for (int i = FIRST_BMAP_BLOCK; i < FIRST_BMAP_BLOCK + NUM_BMAP_BLOCKS; i++){
                rd_block(&bmap[j*BLOCK_SIZE], i, BLOCK_SIZE);
                j++;
        }
}

/**
 * Writes the block bitmap to HD.
 */
void write_bmap()
{
        int j = 0;
        for (int i = FIRST_BMAP_BLOCK; i < FIRST_BMAP_BLOCK + NUM_BMAP_BLOCKS; i++){
                wrt_block(i, &bmap[j*BLOCK_SIZE], BLOCK_SIZE); //TODO: test this!
                j++;
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
               if (is_allocated_block(blk) == FALSE){
                       return blk;
               }
               blk++;
              
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
        
        //dprintf("byte: %b\n", byte);
        
        byte = (byte >> (8-bit)) | (byte << bit); //rotate left
        
        //dprintf("byte: %b\n", byte);
                
        if (flag == TRUE){
                byte = byte | 0x80;  //set first bit to 1 with (byte OR 10000000)
        } else {
                byte = byte & 0x7F;  //set first bit to 0 with (byte AND 01111111) 
        }
        
        //dprintf("byte: %b\n", byte);
                
        byte = (byte >> bit) | (byte << (8-bit)); //rotate right (back)
        
        //dprintf("byte: %b\n\n", byte);
                
        bmap[blk_nr / 8] = byte;
}

bool is_allocated_block(block_nr blk_nr)
{
       uint8 byte = bmap[blk_nr / 8];
       uint8 bit  = blk_nr % 8;
                        
       byte = byte << bit;
       byte = byte & 0x80;
       
       return (byte != 0);
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

void dump_bmap()
{
        uint8 byte = 0; 
        uint8 bit  = 0;
        
        for (block_nr blk_nr = BOOT_BLOCK; blk_nr < NUM_BLOCKS_ON_HD; blk_nr++){
                if (is_allocated_block(blk_nr)){
                        dprintf("block %d is used\n", blk_nr);
                }
                //if (bmap[blk_nr] != 0) dprintf("block group %d: %b\n", blk_nr, bmap[blk_nr]);
        }
}
