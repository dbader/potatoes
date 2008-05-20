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
 * initializes memory management
 *
 * @author Johannes Schamburger
 * @author $LastChangedBy$
 * @version $Rev$
 */

#include "../include/types.h"
#include "../include/stdio.h"
#include "../include/debug.h"
#include "mm_const.h"


//TODO: this should be declared in some header file.
void gdt_init();

/**
 * Initializes memory management (including the GDT)
 * 
 * @param start the start of the part of the memory to be managed
 * @param end the end of the part of the memory to be managed
 */
void mm_init(uint32 start, uint32 end) 
{      
        dprintf("mm: init\n");
        
        gdt_init(); dprintf("mm: GDT initialized\n");
        
        mm_start = start;
        mm_end = end;
        mm_occupied_top = (void*) start;
        
        dprintf("mm: memory 0x%x - 0x%x (%dmb)\n", start, end, (end - start) / (1024 * 1024));
}
