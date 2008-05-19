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
 * Basic block based functions.
 *
 * @author Vincenz Doelle
 * @author $LastChangedBy$
 * @version $Rev$
 */

#include "../include/const.h"
#include "../include/types.h"

#include "const.h"
#include "types.h"


/**
 * central block bitmap from bmap.c
 */
extern bit bmap[NUM_BLOCKS_ON_HD];

/**
 * mark_block from bmap.c
 */
extern void mark_block(block_nr blk, bool flag);

extern block_nr get_free_block(block_nr start);



uint32 read_block(block_nr block, uint16 *buf, uint16 num_bytes);

uint32 write_block(block_nr block, uint16 *buf, uint16 num_bytes);

uint8 free_block(block_nr block);

block_nr alloc_block(block_nr start);

uint32 new_block(); 

/**
 * Allocates a new block.
 * Search linear from the "start" block number.
 */

block_nr alloc_block(block_nr start){
        block_nr blk_nr;
        blk_nr = get_free_block(start);
        mark_block(blk_nr, TRUE);
        return blk_nr;
}
