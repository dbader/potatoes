/* $Id: fs_bmap.h 266 2009-10-14 08:15:36Z dtraytel $
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
 * Basic definitions concerning the block bitmap (bmap)
 *
 * @author Vincenz Doelle
 * @author $LastChangedBy: dtraytel $
 * @version $Rev: 266 $
 */

#ifndef FS_BMAP_H_
#define FS_BMAP_H_

/**
 * Functions on the block bitmap
 */

size_t malloc_bmap();

void init_bmap();

void load_bmap();

void write_bmap();

block_nr get_free_block(block_nr start);

void mark_block(block_nr blk_nr, bool flag);

block_nr alloc_block(block_nr start);

bool is_allocated_block(block_nr blk_nr);

void dump_bmap();

/**
 * Central block bitmap structure.
 *
 * a bit larger than NUM_BLOCKS_ON_HD / 8
 */
/****************************************************/
uint8 *bmap;
uint32 num_bmap_blocks;
uint32 first_data_block;
/****************************************************/

#endif /*FS_BMAP_H_*/
