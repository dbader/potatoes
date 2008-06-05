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
 * Basic buffer functions.
 * Types of temporary buffers used:
 * 
 * 1) current_block_buffer
 * 2) write_pre_cache_buffer
 * 3) current_indirect_adr_buffer
 * 
 * @author Vincenz Doelle
 * @author $LastChangedBy$
 * @version $Rev$
 */

#include "../include/const.h"
#include "../include/types.h"
#include "../include/string.h"

#include "fs_const.h"
#include "fs_types.h"
#include "fs_buf.h"
#include "fs_block_dev.h"

/**
 * Clears a struct of type 'block_cache'.
 * 
 * @param cache Pointer to a cache
 */
void clear_cache(block_cache *cache)
{
        bzero(cache->cache, BLOCK_SIZE);
        cache->block_nr = 0;
}

/**
 * Clears a array of bytes = a buffer.
 * 
 * @param Pointer to the array/buffer
 */
void clear_buffer(uint8 buffer[BLOCK_SIZE])
{
        bzero(buffer, BLOCK_SIZE);
}
