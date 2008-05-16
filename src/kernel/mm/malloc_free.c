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
 * Functions to allocate and free memory
 *
 * @author jschamburger
 * @author $LastChangedBy$
 * @version $Rev$
 */

/*
 * date          stat
 * -----------------------------------------------
 * 10.05.08      first version with a very simple implementation of malloc
 */

#include "../include/types.h"
#include "mm.h"

/**
 * allocates size bytes and additionally saves a name in the header of the block
 */
void *malloc_name(uint32 size, uint8 name[])
{
        if(!mem_occupied_top)
                mem_occupied_top = (void*)START;
        
        mem_occupied_top += size;
        if((uint32)mem_occupied_top < END) {
                return (void*)(mem_occupied_top);
        } else {
                mem_occupied_top -= size;
                return (void*)-1;
        }
}

/** 
 * allocates size bytes
 */ 
void *malloc(uint32 size)
{
        return malloc_name(size,"noname");
}

/**
 * frees the memory in the block starting at address
 */
void free(uint32 address)
{
        ;
}
