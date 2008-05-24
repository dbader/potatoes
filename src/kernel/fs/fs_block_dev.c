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

#include "fs_const.h"
#include "fs_types.h"
#include "fs_buf.h"

#include "../include/string.h"
#include "../include/stdio.h"


extern void read_sector(uint32 source, void* target);
extern void hd_read_sector(uint16* dest, uint32 src);
extern void hd_write_sector(uint32 dest, uint16* src);

/**
 * Read a block from HD into buf and cache it into the read_cache.
 * 
 * @param block         The block number which should be read
 * @param *buf          A pointer to the dest. buffer
 * @param num_bytes     Number of bytes that should be read
 */
void rd_block(block_nr blk_nr, void *buf, size_t num_bytes)
{       
        block_cache *cache = get_read_cache();
        clear_cache(cache);
        
        hd_read_sector((uint16*)cache->cache, blk_nr);                            //get block from IO 
       
        cache->block_nr = blk_nr;                                        //remember the block number
        
        memcpy(buf, cache->cache, num_bytes);                            //copy block to destination
}

void wrt_block(block_nr blk_nr, void *buf, size_t num_bytes)
{
        block_cache *cache = get_write_cache();
        clear_cache(cache);
        
        cache->block_nr = blk_nr;

        memcpy(cache->cache, buf, num_bytes);
        
        hd_write_sector(blk_nr, (uint16*)cache->cache);
}

void wrt_cache(block_cache *cache, size_t num_bytes)
{
        wrt_block(cache->block_nr, cache->cache, num_bytes);
}



