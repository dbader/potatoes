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
#include "../include/string.h"
#include "mm.h"
#include "../io/io_virtual.h"

/**
 * Initializes memory management (including the GDT)
 * 
 * @param start the start of the part of the memory to be managed
 * @param end   the end of the part of the memory to be managed
 */
void mm_init(uint32 start, uint32 end) 
{      
        
        // set the start and the end of the memory
        mm_start = (mm_header*) start;
        mm_end = (mm_header*) (end - sizeof(mm_header));
//        mm_start2 = (mm_header*) (0x1F00000);
        
        // set the headers for mm_start and mm_end (both have size = 0; just needed for linked list stuff)
        mm_start->next = mm_end;
        mm_start->prev = mm_end;
        mm_start->name = "start";
        mm_start->size = 0;
        
        mm_end->next = mm_start;
        mm_end->prev = mm_start;
        mm_end->name = "end";
        mm_end->size = 0;
        
        start_vmonitor();
                
        printf("etiOS - $Rev: 119 $ - "__DATE__" "__TIME__"\n");
//        dprintf("mm_start.next: 0x%x\n",(*mm_start).next);
//        dprintf("mm_start.prev: 0x%x\n",(*mm_start).prev);
//        dprintf("mm_start.name: %s\n",(*mm_start).name);
//        dprintf("mm_start.size: %d\n",(*mm_start).size);
        dprintf("-----------------------------------------------"
                        "---------------------------------mm: init\n");
        dprintf("size of struct mm_header: %d\n",sizeof(mm_header));
        gdt_init(); dprintf("mm: GDT initialized\n");  
//        dprintf("mm_end.next: 0x%x\n",(*mm_end).next);
//        dprintf("mm_end.prev: 0x%x\n",(*mm_end).prev);
//        dprintf("mm_end.name: %s\n",(*mm_end).name);
//        dprintf("mm_end.size: %d\n",(*mm_end).size);
        
        dprintf("mm: memory 0x%x - 0x%x (%dmb)\n", start, end, (end - start) / (1024 * 1024));
}


/** 
 * moves a block of memory (including the header)
 * TODO implementation that keeps the entries sorted
 * 
 * @param dest  destination
 * @param src   source
 */
void mm_move_block(uint32 dest, mm_header *src)
{
        mm_header* new_header = (mm_header*) dest;
        
        (src->prev)->next = new_header;
        (src->next)->prev = new_header;
        memcpy((void*)(new_header), (void*)src, (sizeof(mm_header) + src->size));
        //printf("\n%s (0x%x)\nnext: 0x%x\nprev: 0x%x\nsize: %d\n", new_header->name, (uint32)new_header, (uint32)new_header->next, (uint32)new_header->prev, (uint32)new_header->size);
}
