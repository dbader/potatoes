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
 * Builds and initializes the interrupt descriptor table.
 *
 * @author Dmitriy Traytel
 * @author $LastChangedBy$
 * @version $Rev$
 */

#include "../include/types.h"

struct idt_entry //16+16+8+8+16=64 bit
{
        uint16 low_offset;
        uint16 selector;
        uint8 separator; //always 0
        uint8 flags;
        uint16 high_offset;
}__attribute__((packed)); //gcc-flag to use 64 connected bits of memory for the struct 

struct idt_pointer //16+32=48 bit
{
        uint16 maxsize;
        uint32 start;
}__attribute__((packed)); //gcc-flag to use 64 connected bits of memory for the struct

struct idt_entry idt[256]; //our idt with 256 descriptors
struct idt_pointer idtp;

void idt_fill_entry(uint8 pos, uint32 offset, uint16 sel, uint8 flg)
{
        idt[pos].selector = sel;
        idt[pos].flags = flg;
        idt[pos].separator = 0;
        idt[pos].low_offset = offset % 65536;
        idt[pos].high_offset = offset / 65536;
}

extern void idt_load();

void idt_init()
{
        idtp.maxsize = (64 * 256) - 1;
        idtp.start = (uint32) &idt;
        int i;
        for (i=0; i<256; i++){                  //fill with empty descriptors
                idt_fill_entry(i,0,0,0);
        }
        idt_load();
}
