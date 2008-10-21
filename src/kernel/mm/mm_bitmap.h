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
 * Declarations of the functions needed for frame allocation.
 * 
 * @author Johannes Schamburger
 * @author $LastChangedBy $
 * @version $Rev $
 */

#include "../include/types.h"
#include "../include/stdio.h"
#include "mm_paging.h"

#ifndef __FRAME_H_
#define __FRAME_H_

void set_frame(uint32 frame_addr);
void clear_frame(uint32 frame_addr);
bool test_frame(uint32 frame_addr);
uint32 first_free_frame();

void alloc_frame(page_t *page, int kernel, int writeable);
void free_frame(page_t *page);

void bitset_test();

#endif /*__FRAME_H_*/
