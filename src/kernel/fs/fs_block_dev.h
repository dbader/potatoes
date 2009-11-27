/* $Id: fs_block_dev.h 266 2009-10-14 08:15:36Z dtraytel $
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
 * Basic definitions of all block based functions.
 *
 * @author Vincenz Doelle
 * @author $LastChangedBy: dtraytel $
 * @version $Rev: 266 $
 */

#ifndef __FS_BLOCK_DEV_H_
#define __FS_BLOCK_DEV_H_

void rd_block(void *buf, block_nr blk_nr, size_t num_bytes);

void cache_block(block_nr blk_nr, size_t num_bytes);

void wrt_block(block_nr blk_nr, void *buf, size_t num_bytes);

void wrt_cache(block_cache *cache, size_t num_bytes);

void clear_block(block_nr blk_nr);

block_nr get_data_block(m_inode *inode, uint32 pos, bool allow_enlargement);

block_nr enlarge_file(block_nr *blk_ptr, block_nr alloc_start);

#endif /*__FS_BLOCK_DEV_H_*/
