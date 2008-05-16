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
 * Definitions for the functions used in the memory manager.
 *
 * @author jschamburger
 * @author $LastChangedBy$
 * @version $Rev$
 */

#include "../include/types.h"

#ifndef MM_H_
#define MM_H_

// memory for applications starts at 10 MB and ends at 32 MB
const uint32 START = 0x009FFFFF;
const uint32 END = 0x01FFFFFF;
void *mem_occupied_top;

// allocates size bytes 
extern void *malloc(uint32 size);

// allocates size bytes and additionally saves a name in the header of the block
extern void *malloc_name(uint32 size, uint8 name[]);

// frees the memory in the block starting at address
extern void free(uint32 address);

#endif /*MM_H_*/
