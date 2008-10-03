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
#include "fs_inode_table.h"

#include "../include/string.h"
#include "../include/stdio.h"
#include "../io/io_harddisk.h"



/**
 * Read a block from HD into buf and cache it into the read_cache.
 *
 * @param block         block number to be read
 * @param *buf          pointer to the dest. buffer
 * @param num_bytes     number of bytes to be read
 */
void rd_block(void *buf, block_nr blk_nr, size_t num_bytes)
{
        cache_block(blk_nr, num_bytes);
        memcpy(buf, read_cache.cache, num_bytes);                            //copy block to destination
}

/**
 * Load a block from HD into temporary read_cache.
 *
 * @param blk_nr        block number to be read
 * @param num_bytes     number of bytes to be read
 */
void cache_block(block_nr blk_nr, size_t num_bytes)
{
        clear_cache(&read_cache);

        hd_read_sector(read_cache.cache, blk_nr);                            //get block from IO

        read_cache.block_nr = blk_nr;                                        //remember the block number
}

/**
 * Write the buffer's content to HD.
 *
 * @param blk_nr        destination block number
 * @param buf           buffer with content to be written
 * @param num_bytes     number of bytes to be written
 */
void wrt_block(block_nr blk_nr, void *buf, size_t num_bytes)
{
        clear_cache(&write_cache);

        write_cache.block_nr = blk_nr;

        memcpy(write_cache.cache, buf, num_bytes);

        hd_write_sector(blk_nr, write_cache.cache);
}

/**
 * Write a block cache to HD.
 *
 * @param cache         cache with content
 * @param num_bytes     number of bytes to be written
 */
void wrt_cache(block_cache *cache, size_t num_bytes)
{
        wrt_block(cache->block_nr, cache->cache, num_bytes);
}

/**
 * Clear a block from HD by resetting it with zeros.
 *
 * @param blk_nr        block number to be reset
 */
void clear_block(block_nr blk_nr)
{
        rd_block(read_cache.cache, blk_nr, BLOCK_SIZE);
        read_cache.block_nr = blk_nr;

        bzero(read_cache.cache, BLOCK_SIZE);

        wrt_cache(&read_cache, BLOCK_SIZE);
}

/**
 * Determine block for given position.
 *
 * @param inode         file's inode
 * @param pos           position within the file
 * @param allow_scaling enlarge the file if pos > EOF?
 * @return              block number containing the desired position
 */
block_nr get_data_block(m_inode *inode, uint32 pos, bool allow_scaling)
{
        fs_dprintf("[fs_block_dev] get_data_block(%d, %d, %d)\n", inode->i_adr, pos, allow_scaling);

        uint32   blk_nr;
        block_nr data_blk;

        //data can be found by direct address pointer
        if (pos < BYTES_DIRECT) {
                blk_nr = pos / BLOCK_SIZE; //block numbers start with 0, 1, 2, ...
                data_blk = inode->i_direct_pointer[blk_nr];

                if (data_blk == NULL) {
                        if (!allow_scaling) {
                                return NOT_EXISTENT;
                        }

                        data_blk = scale(&(inode->i_direct_pointer[blk_nr]), inode->i_adr);

                        fs_dprintf("[fs_block_dev] dp == NULL --> dp[%d] = %d\n", blk_nr, data_blk, inode->i_direct_pointer[blk_nr]);

                }

                //data can be found by single indirect pointer
        } else if (pos < (BYTES_DIRECT + BYTES_SINGLE_INDIRECT)) {
                block_nr sip = inode->i_single_indirect_pointer;

                if (sip == NULL) {
                        if (!allow_scaling) {
                                return NOT_EXISTENT;
                        }

                        sip = scale(&(inode->i_single_indirect_pointer), inode->i_adr);

                        fs_dprintf("[fs_block_dev] sip == NULL --> sip = %d\n", sip);
                }

                rd_block(addr_cache, inode->i_single_indirect_pointer, sizeof(addr_cache));

                blk_nr = (pos - BYTES_DIRECT) / BLOCK_SIZE;  //calc pos offset from first indirect block on
                data_blk = addr_cache[blk_nr];

                if (data_blk == NULL) {
                        if (!allow_scaling) {
                                return NOT_EXISTENT;
                        }

                        data_blk = scale(&addr_cache[blk_nr], inode->i_single_indirect_pointer);

                        wrt_block(inode->i_single_indirect_pointer, addr_cache, sizeof(addr_cache)); //write changes

                        fs_dprintf("[fs_block_dev] addr_cache[%d] in %d == NULL --> addr_cache[%d] = %d\n", blk_nr, inode->i_single_indirect_pointer, blk_nr, data_blk);
                }

                //data can only be found by double indirect pointer
        } else if (pos < (BYTES_DIRECT + BYTES_SINGLE_INDIRECT + BYTES_DOUBLE_INDIRECT)) {
                block_nr dip = inode->i_double_indirect_pointer;

                if (dip == NULL) {
                        if (!allow_scaling) {
                                return NOT_EXISTENT;
                        }

                        dip = scale(&(inode->i_double_indirect_pointer), inode->i_adr);

                        fs_dprintf("[fs_block_dev] dip == NULL --> dip = %d\n", dip);
                }

                rd_block(addr_cache, inode->i_double_indirect_pointer, sizeof(addr_cache));

                blk_nr = (pos - BYTES_DIRECT - BYTES_SINGLE_INDIRECT) / BLOCK_SIZE; //block nr with the desired data
                block_nr addr_block = blk_nr / (ADDRS_PER_BLOCK); //address block pointing to the data block

                if (addr_cache[addr_block] == NULL) {
                        if (!allow_scaling) {
                                return NOT_EXISTENT;
                        }

                        block_nr new_addr_block = scale(&addr_cache[addr_block], inode->i_double_indirect_pointer);

                        wrt_block(inode->i_double_indirect_pointer, addr_cache, sizeof(addr_cache)); //write changes

                        fs_dprintf("[fs_block_dev] addr_cache[%d] == NULL --> addr_cache[%d] = %d\n", addr_block, addr_block, new_addr_block);
                }
                rd_block(addr_cache, addr_cache[addr_block], sizeof(addr_cache));

                data_blk = addr_cache[blk_nr % ADDRS_PER_BLOCK];

                if (data_blk == NULL) {
                        if (!allow_scaling) {
                                return NOT_EXISTENT;
                        }

                        data_blk = scale(&addr_cache[blk_nr % ADDRS_PER_BLOCK], inode->i_double_indirect_pointer);

                        wrt_block(inode->i_double_indirect_pointer, addr_cache, sizeof(addr_cache)); //write changes

                        fs_dprintf("[fs_block_dev] addr_cache[%d \% ADDRS_PER_BLOCK] == NULL --> addr_cache[%d\% ADDRS_PER_BLOCK] = %d\n", blk_nr, blk_nr, data_blk);
                }

        } else {
                return NOT_FOUND;
        }

        write_inode(inode); //could be changed because of scaling

        return data_blk; //annotation: inode can be changed!
}

/**
 * Enlarge/scale the file.
 *
 * @param blk_ptr       pointer to inode block pointer
 * @param alloc_start   start block for block allocation search
 * @return block_nr     number of newly allocated block
 */
block_nr scale(block_nr *blk_ptr, block_nr alloc_start)
{
        block_nr data_blk = alloc_block(alloc_start);
        clear_block(data_blk);
        *blk_ptr = data_blk;

        return data_blk;
}


