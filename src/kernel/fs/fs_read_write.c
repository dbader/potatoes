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
 * Functions concerning syscalls "read" and "write"
 * 
 * @author Vincenz Doelle
 * @author $LastChangedBy$
 * @version $Rev$
 */
#include "../include/const.h"
#include "../include/types.h"
#include "../include/string.h"
#include "../include/stdlib.h"

#include "fs_const.h"
#include "fs_types.h"
#include "fs_block_dev.h"
#include "fs_buf.h"

bool fs_read(void *buf, m_inode *inode, size_t num_bytes, uint32 pos);

bool fs_read(void *buf, m_inode *inode, size_t num_bytes, uint32 pos)
{
        block_nr main_blk = inode->i_adr;
        rd_block(&d_inode_cache, main_blk, sizeof(d_inode));
        
        block_nr blk;
        block_nr blk_nr;
        uint16 offset;
        
        if (pos < BYTES_DIRECT){
                blk = pos / BLOCK_SIZE; //block numbers start with 0, 1, 2, ...
                blk_nr = inode->i_direct_pointer[blk];
                
        } else if (pos < BYTES_DIRECT + BYTES_SINGLE_INDIRECT){
                rd_block(addr_cache, inode->i_single_indirect_pointer, BLOCK_SIZE);
                                
                blk = (pos - BYTES_DIRECT) / BLOCK_SIZE;  //calc pos offset from first indirect block on
                blk_nr = addr_cache[blk];
                
        } else if (pos < BYTES_DIRECT + BYTES_SINGLE_INDIRECT + BYTES_DOUBLE_INDIRECT){
                rd_block(addr_cache, inode->i_double_indirect_pointer, BLOCK_SIZE);
                
                blk = (pos - BYTES_DIRECT - BYTES_SINGLE_INDIRECT) / BLOCK_SIZE;
                block_nr addr_block = blk / (ADDRS_PER_BLOCK);
                
                rd_block(addr_cache, addr_cache[addr_block], BLOCK_SIZE);
                
                blk_nr = addr_cache[blk % addr_block];
        } else {
                return FALSE;
        }
        
        rd_block(read_cache.cache, blk_nr, BLOCK_SIZE);
        read_cache.block_nr = blk_nr;
        
        offset = pos % BLOCK_SIZE;
        if (offset + num_bytes < BLOCK_SIZE){
                memcpy(buf, read_cache.cache + offset, num_bytes); //TODO: counter-check read_cache.cache + offset
        } else {
                uint16 num_readable_bytes = BLOCK_SIZE - offset;
                memcpy(buf, read_cache.cache + offset, num_readable_bytes);
                
                fs_read(buf + num_readable_bytes, inode, num_bytes - (num_readable_bytes), pos + num_readable_bytes);
        }
        
        return TRUE;
}
