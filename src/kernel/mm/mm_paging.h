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
 * Declarations for the functions and variables needed for paging.
 * 
 * @author Johannes Schamburger
 * @author $LastChangedBy $
 * @version $Rev $
 */

#include "../include/types.h"
#include "../pm/pm_main.h"

#ifndef __PAGING_H_
#define __PAGING_H_


typedef struct page
{
        uint32 present  : 1;    // 1: page is present in memory; 0: not present
        uint32 rw       : 1;    // 1: readable and writeable; 0: read-only
        uint32 user     : 1;    // 1: accessible from user mode; 0: only accessible from supervisor
        uint32 res      : 2;    // reserved by the CPU
        uint32 accessed : 1;    // 1: page has been accessed since last refresh
        uint32 dirty    : 1;    // 1: page been written to since last refresh
        uint32 res2     : 2;    // reserved by the CPU
        uint32 avail    : 3;    // available for kernel use
        uint32 frame    : 20;   // frame address (least significant 12 bits are 0 because each frame
                                // is fixed at 4KB in size  
} page_t;

typedef struct page_table
{
   page_t pages[1024];
} page_table_t;

typedef struct page_directory
{
   page_table_t *tables[1024];  // pointers to the pagetables
   uint32 tablesPhysical[1024]; // pointers to the pagetables (physical adresses!)
   uint32 physicalAddr;         // physical address of tablesPhysical
} page_directory_t;

void switch_page_dir(page_directory_t *new);
page_t *get_page(uint32 address, int make, page_directory_t *dir);
void page_fault(cpu_state_t state);

uint32 placement_addr;

page_directory_t *kernel_dir;
page_directory_t *current_dir;

uint32 *frames;
uint32 nframes;

uint32 kmalloc(uint32 size, int page_aligned, uint32 *phys);

// asm-functions from mm_util.s
void write_to_cr0(uint32 cr0);
uint32 read_from_cr0();
void write_to_cr3(uint32 cr3);
uint32 read_from_cr3();
#endif /*__PAGING_H_*/
