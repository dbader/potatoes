/* $Id: fs_buf.c 266 2009-10-14 08:15:36Z dtraytel $
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
 * Basic buffer functions.
 * 
 * Types of temporary caches (= [addr][buffer]) used:
 * 1) read_cache
 * 2) write_cache
 * 3) addr_cache
 * 4) d_inode_cache
 * 5) m_inode_cache
 *
 * @author Vincenz Doelle
 * @author $LastChangedBy: dtraytel $
 * @version $Rev: 266 $
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
 * @param cache pointer to a cache
 */
void clear_cache(block_cache *cache)
{
        bzero(cache->cache, BLOCK_SIZE);
        cache->block_nr = 0;
}

/**
 * Clears a array of bytes = a buffer.
 *
 * @param buffer  pointer to the array/buffer
 */
void clear_buffer(uint8 buffer[BLOCK_SIZE])
{
        bzero(buffer, BLOCK_SIZE);
}
