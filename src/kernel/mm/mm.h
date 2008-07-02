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
 * Definitions for the functions and variables used in the memory manager.
 * 
 * @author Johannes Schamburger
 * @author $LastChangedBy$
 * @version $Rev$
 */

#include "../include/types.h"

#ifndef __MM_H_
#define __MM_H_

/**
 * the structure of a header of an occupied memory block (size: 4 * 32bit = 128bit
 */
typedef struct mm_header {
        struct mm_header *prev;         // pointer to the header of the previous occupied block
        struct mm_header *next;         // pointer to the header of the next occupied block
        char name[32];                     // name of the block
        uint32 size;                    // size of the current block
       // uint8 pad[100];
} mm_header;

/**
 * the start adress of the part of the memory that shall be managed
 */
mm_header *mm_start;
// [temporary] start of the part of the memory used for testing
mm_header *mm_start_test;

/**
 * the end adress of the part of the memory that shall be managed
 */
mm_header *mm_end;

void gdt_init();

void mm_move_block(uint32 dest, mm_header *src);

#endif /* __MM_H_ */
