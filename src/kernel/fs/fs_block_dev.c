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
 * Read a block from HD into 'buf' and cache it into the read_cache.
 *
 * @param *buf          pointer to the dest. buffer
 * @param blk_nr        block number to be read
 * @param num_bytes     number of bytes to be read
 */
void rd_block(void *buf, block_nr blk_nr, size_t num_bytes)
{
        cache_block(blk_nr, num_bytes); //read BLOCK_SIZE bytes from HD to cache
        memcpy(buf, read_cache.cache, num_bytes); //copy requested number of bytes to destination
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

        hd_read_sector(read_cache.cache, blk_nr); //get block from IO (sector = block)

        read_cache.block_nr = blk_nr; //remember the block number
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
 * Determine block for given position within an inode.
 *
 * @param inode              file's inode
 * @param pos                position/byte within the file/inode
 * @param allow_enlargement  enlarge the file if pos > EOF?
 * @return                   block number containing the desired position
 */
block_nr get_data_block(m_inode *inode, uint32 pos, bool allow_enlargement)
{
        fs_dprintf("[fs_block_dev] get_data_block(%d, %d, %d)\n", inode->i_adr, pos, allow_enlargement);

        uint32   blk_nr;
        block_nr data_blk;

        //data can be found by direct address pointer
        if (pos < BYTES_DIRECT) {
                blk_nr = pos / BLOCK_SIZE; //block numbers start with 0, 1, 2, ...
                data_blk = inode->i_direct_pointer[blk_nr];

                if (data_blk == NULL) {
                        if (!allow_enlargement) {
                                return NOT_EXISTENT;
                        }

                        data_blk = enlarge_file(&(inode->i_direct_pointer[blk_nr]), inode->i_adr);

                        fs_dprintf("[fs_block_dev] dp == NULL --> dp[%d] = %d\n", blk_nr, data_blk, inode->i_direct_pointer[blk_nr]);

                }

          //data can be found by single indirect pointer
        } else if (pos < (BYTES_DIRECT + BYTES_SINGLE_INDIRECT)) {
                block_nr sip = inode->i_single_indirect_pointer;

                if (sip == NULL) {
                        if (!allow_enlargement) {
                                return NOT_EXISTENT;
                        }

                        sip = enlarge_file(&(inode->i_single_indirect_pointer), inode->i_adr);

                        fs_dprintf("[fs_block_dev] sip == NULL --> sip = %d\n", sip);
                }

                rd_block(addr_cache, inode->i_single_indirect_pointer, sizeof(addr_cache));

                blk_nr = (pos - BYTES_DIRECT) / BLOCK_SIZE;  //calc pos offset from first indirect block on
                data_blk = addr_cache[blk_nr];

                if (data_blk == NULL) {
                        if (!allow_enlargement) {
                                return NOT_EXISTENT;
                        }

                        data_blk = enlarge_file(&addr_cache[blk_nr], inode->i_single_indirect_pointer);

                        wrt_block(inode->i_single_indirect_pointer, addr_cache, sizeof(addr_cache)); //write changes

                        fs_dprintf("[fs_block_dev] addr_cache[%d] in %d == NULL --> addr_cache[%d] = %d\n", blk_nr, inode->i_single_indirect_pointer, blk_nr, data_blk);
                }

          //data can only be found by double indirect pointer
        } else if (pos < (BYTES_DIRECT + BYTES_SINGLE_INDIRECT + BYTES_DOUBLE_INDIRECT)) {
                block_nr dip = inode->i_double_indirect_pointer;

                if (dip == NULL) {
                        if (!allow_enlargement) {
                                return NOT_EXISTENT;
                        }

                        dip = enlarge_file(&(inode->i_double_indirect_pointer), inode->i_adr);

                        fs_dprintf("[fs_block_dev] dip == NULL --> dip = %d\n", dip);
                }

                rd_block(addr_cache, dip, sizeof(addr_cache));

                blk_nr = (pos - BYTES_DIRECT - BYTES_SINGLE_INDIRECT) / BLOCK_SIZE; //block nr with the desired data
                block_nr addr_block = blk_nr / ADDRS_PER_BLOCK; //address block pointing to the data block

                block_nr sip = addr_cache[addr_block];

                if (sip == NULL) {
                        if (!allow_enlargement) {
                                return NOT_EXISTENT;
                        }

                        sip = enlarge_file(&addr_cache[addr_block], dip);

                        wrt_block(dip, addr_cache, sizeof(addr_cache)); //write changes

                        fs_dprintf("[fs_block_dev] addr_cache[%d] == NULL --> addr_cache[%d] = %d\n", addr_block, addr_block, sip);
                }

                rd_block(addr_cache, sip, sizeof(addr_cache));

                data_blk = addr_cache[blk_nr % ADDRS_PER_BLOCK];

                if (data_blk == NULL) {
                        if (!allow_enlargement) {
                                return NOT_EXISTENT;
                        }

                        data_blk = enlarge_file(&addr_cache[blk_nr % ADDRS_PER_BLOCK], dip);

                        wrt_block(sip, addr_cache, sizeof(addr_cache)); //write changes

                        fs_dprintf("[fs_block_dev] addr_cache[%d %% ADDRS_PER_BLOCK] == NULL --> addr_cache[%d %% ADDRS_PER_BLOCK] = %d\n", blk_nr, blk_nr, data_blk);
                }

        } else {
                return NOT_FOUND;
        }

        write_inode(inode); //could be changed because of enlargement

        return data_blk; //annotation: inode can be changed!
}

/**
 * Enlarge the file.
 *
 * @param blk_ptr       pointer to inode block pointer
 * @param alloc_start   start block for block allocation search
 * 
 * @return block_nr     number of newly allocated block
 */
block_nr enlarge_file(block_nr *blk_ptr, block_nr alloc_start)
{
        block_nr data_blk = alloc_block(alloc_start);
        clear_block(data_blk);
        *blk_ptr = data_blk; //update pointer in inode

        return data_blk;
}


