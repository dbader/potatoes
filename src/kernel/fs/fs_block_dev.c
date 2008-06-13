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
#include "../include/debug.h"

#include "fs_const.h"
#include "fs_types.h"
#include "fs_buf.h"
#include "fs_bmap.h"
#include "fs_block_dev.h"

#include "../include/string.h"
#include "../include/stdio.h"
#include "../io/io_harddisk.h"



/**
 * Read a block from HD into buf and cache it into the read_cache.
 * 
 * @param block         The block number which should be read
 * @param *buf          A pointer to the dest. buffer
 * @param num_bytes     Number of bytes that should be read
 */
void rd_block(void *buf, block_nr blk_nr, size_t num_bytes)
{       
        cache_block(blk_nr, num_bytes);
        memcpy(buf, read_cache.cache, num_bytes);                            //copy block to destination
}

void cache_block(block_nr blk_nr, size_t num_bytes)
{
        clear_cache(&read_cache);
        
        hd_read_sector(read_cache.cache, blk_nr);                            //get block from IO 
       
        read_cache.block_nr = blk_nr;                                        //remember the block number
}

void wrt_block(block_nr blk_nr, void *buf, size_t num_bytes)
{
        clear_cache(&write_cache);
        
        write_cache.block_nr = blk_nr;

        memcpy(write_cache.cache, buf, num_bytes);
        
        hd_write_sector(blk_nr, write_cache.cache);
}

void wrt_cache(block_cache *cache, size_t num_bytes)
{
        wrt_block(cache->block_nr, cache->cache, num_bytes);
}

void clear_block(block_nr blk_nr)
{
        rd_block(read_cache.cache, blk_nr, BLOCK_SIZE);
        read_cache.block_nr = blk_nr;
        
        bzero(read_cache.cache, BLOCK_SIZE);
        wrt_cache(&read_cache, BLOCK_SIZE);
}

block_nr get_data_block(m_inode *inode, uint32 pos)
{
        dprintf("[fs_block_dev] get_data_block(%d, %d)\n", inode->i_adr, pos);
        block_nr blk_nr;
        block_nr data_blk;

        
        if (pos < BYTES_DIRECT){ //data can be found by direct address pointer
                blk_nr = pos / BLOCK_SIZE; //block numbers start with 0, 1, 2, ...
                data_blk = inode->i_direct_pointer[blk_nr];
                
                if (data_blk == NULL){
                        data_blk = alloc_block(inode->i_adr); //init inode's direct pointer
                        clear_block(data_blk);
                        inode->i_direct_pointer[blk_nr] = data_blk;
                        
                        dprintf("[fs_block_dev] dp == NULL --> dp[%d] = %d\n", blk_nr, data_blk);
                }
                
        } else if (pos < (BYTES_DIRECT + BYTES_SINGLE_INDIRECT)){ //data can be found by single indirect pointer
                block_nr sip = inode->i_single_indirect_pointer;
                
                if (sip == NULL){
                        sip = alloc_block(inode->i_adr); //init inode's single indirect pointer
                        clear_block(sip);
                        inode->i_single_indirect_pointer = sip;
                        
                        dprintf("[fs_block_dev] sip == NULL --> sip = %d\n", sip);
                }
                
                rd_block(addr_cache, inode->i_single_indirect_pointer, sizeof(addr_cache));
                                
                blk_nr = (pos - BYTES_DIRECT) / BLOCK_SIZE;  //calc pos offset from first indirect block on
                data_blk = addr_cache[blk_nr];
                
        } else if (pos < (BYTES_DIRECT + BYTES_SINGLE_INDIRECT + BYTES_DOUBLE_INDIRECT)){
                block_nr dip = inode->i_double_indirect_pointer;
                
                if (dip == NULL){
                        dip = alloc_block(inode->i_adr);
                        inode->i_double_indirect_pointer = dip;
                        clear_block(dip);
                        
                        dprintf("[fs_block_dev] dip == NULL --> dip = %d\n", dip);
                }
                
                rd_block(addr_cache, inode->i_double_indirect_pointer, sizeof(addr_cache));
                
                blk_nr = (pos - BYTES_DIRECT - BYTES_SINGLE_INDIRECT) / BLOCK_SIZE; //block nr with the desired data
                block_nr addr_block = blk_nr / (ADDRS_PER_BLOCK); //address block pointing to the data block
                
                if (addr_cache[addr_block] == NULL){
                        block_nr new_addr_block = alloc_block(inode->i_double_indirect_pointer);
                        clear_block(new_addr_block);
                        addr_cache[addr_block] = new_addr_block;
                        wrt_block(inode->i_double_indirect_pointer, addr_cache, sizeof(addr_cache)); //write changes
                        
                        dprintf("[fs_block_dev] addr_cach[%d] == NULL --> addr_cache[%d] = %d\n", addr_block, addr_block, new_addr_block);
                }
                rd_block(addr_cache, addr_cache[addr_block], sizeof(addr_cache));
                
                data_blk = addr_cache[blk_nr % ADDRS_PER_BLOCK];
        } else {
                return NOT_FOUND;
        }
        
        return data_blk; //annotation: blk_nr can be an empty block (filled by 0), inode can be changed!
}



