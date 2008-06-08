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
 * Basic definitions of all block based functions.
 *
 * @author Vincenz Doelle
 * @author $LastChangedBy$
 * @version $Rev$
 */
 
#ifndef __FS_BLOCK_DEV_H_
#define __FS_BLOCK_DEV_H_

void rd_block(void *buf, block_nr blk_nr, size_t num_bytes);

void cache_block(block_nr blk_nr, size_t num_bytes);

void wrt_block(block_nr blk_nr, void *buf, size_t num_bytes);

void wrt_cache(block_cache *cache, size_t num_bytes);

void clear_block(block_nr blk_nr);

block_nr get_data_block(m_inode *inode, uint32 pos);


#endif /*__FS_BLOCK_DEV_H_*/
