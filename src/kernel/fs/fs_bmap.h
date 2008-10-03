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
 * Basic definitions concerning the block bitmap (bmap)
 *
 * @author Vincenz Doelle
 * @author $LastChangedBy$
 * @version $Rev$
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
