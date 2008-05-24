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
 * Basic buffer/cache definitions.
 * 
 * @author Vincenz Doelle
 * @author $LastChangedBy$
 * @version $Rev$
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
 * A directory cache.
 * 
 * (4 bytes for block_nr, NAME_SIZE bytes for the name)
 */
dir_entry dir_cache[BLOCK_SIZE/(4 + NAME_SIZE)];

/*
 * The cache for a block containing indirect addresses.
 * (current indirect address block)
 */
block_cache adr_cache;


/***********************************************************************/

void clear_cache(block_cache *cache);

void clear_buffer(uint8 buffer[BLOCK_SIZE]);

uint8* get_write_buffer();

uint8* get_read_buffer();

block_cache* get_write_cache();

block_cache* get_read_cache();

block_cache* get_adr_cache();

dir_entry* get_dir_cache();

d_inode* get_d_inode_cache();

#endif /*__FS_BUF_H_*/
