/* $Id: fs_buf.h 266 2009-10-14 08:15:36Z dtraytel $
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
 * Basic buffer/cache definitions.
 * (cache = [addr][buffer])
 *
 * @author Vincenz Doelle
 * @author $LastChangedBy: dtraytel $
 * @version $Rev: 266 $
 */


#ifndef __FS_BUF_H_
#define __FS_BUF_H_

/**
 * A flat buffer/ byte array.
 */
uint8 write_buffer[BLOCK_SIZE];

/**
 * A flat buffer/ byte array.
 */
uint8 read_buffer[BLOCK_SIZE];

/**
 * A cache for reading a block.
 */
block_cache read_cache;

/**
 * A cache for writing a block.
 */
block_cache write_cache;

/**
 *  A disk inode cache.
 */
d_inode d_inode_cache;

/**
 *  A memory inode cache.
 */
m_inode m_inode_cache;

/**
 * A directory cache.
 *
 * (4 bytes for block_nr, NAME_SIZE bytes for the name)
 */
dir_entry dir_cache[DIR_ENTRIES_PER_BLOCK];

/*
 * The cache for a block containing indirect addresses.
 * (current indirect address block)
 */
block_nr addr_cache[ADDRS_PER_BLOCK];


/***********************************************************************/

void clear_cache(block_cache *cache);

void clear_buffer(uint8 buffer[BLOCK_SIZE]);

#endif /*__FS_BUF_H_*/
