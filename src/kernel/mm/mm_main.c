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
 * Initializes memory management.
 *
 * @author Johannes Schamburger
 * @author $LastChangedBy$
 * @version $Rev$
 */

#include "../include/types.h"
#include "../include/stdio.h"
#include "../include/stdlib.h"
#include "../include/debug.h"
#include "../include/string.h"
#include "mm.h"
#include "mm_paging.h"
#include "mm_bitmap.h"
#include "../io/io_virtual.h"

/**
 * initializes memory management (includeing the GDT)
 * 
 * @param start start address of the free memory (= end of kernel)
 * @param end end address of the available memory
 */
void mm_init(uint32 start, uint32 end)
{
        placement_addr = start + 0x100; // some memory is reserved for the heap structure 
        nframes = end / 0x1000;
        
        // only 1 bit per frame is needed
        frames = (uint32*)kmalloc(nframes/32, 0, 0);
        bzero(frames,nframes/32);
        
        int j = 0;
        
        kernel_dir = (page_directory_t*)kmalloc(sizeof(page_directory_t), 1, 0);
        bzero(kernel_dir, sizeof(page_directory_t));
        current_dir = kernel_dir;
        
        uint32 start_size = KHEAP_INITIAL_SIZE;
        int i = 0;
        for (i = KHEAP_START; i < KHEAP_START + start_size; i += 0x1000) {
                get_page(i, 1, kernel_dir); 
        }
        
        // allocate frames for kernel code (physical address = virtual address!) 
        i = 0;
        // if new page-tables have to be created, they are stored at the end of the kernel (by kmalloc())
        // so we have to reserve some space
        uint32 max_size_of_page_tables = end / 4096 / 1024 * sizeof(page_table_t);
        
        while (i <= placement_addr + max_size_of_page_tables) {
                // Kernel code is readable but not writeable from userspace.
                alloc_frame( get_page(i, 1, kernel_dir), 0, 0);
                i += 0x1000;
        }
        
        for (i = KHEAP_START; i < KHEAP_START + start_size; i += 0x1000) {
                alloc_frame( get_page(i, 1, kernel_dir), 0, 0);
        }
        
        switch_page_dir(kernel_dir); // enable paging
        
        kernel_heap = create_heap(KHEAP_START, KHEAP_START + start_size, KHEAP_START + end, 0, 0, start);
        
}
/**
 * Second part of the memory management initialization.
 * (must be called after init_vmonitors() in main();
 * the GDT initialization must be here, too, because it
 * uses dprintf() )
 */
void mm_init_output() {
        printf("%{POTATOES - $Rev$ - "__DATE__" "__TIME__"\n", LIGHTGREEN);
        dprint_separator();

        dprintf("%{mm:} init\n", LIGHTBLUE);
        //dprintf("size of struct mm_header: %d\n",sizeof(mm_header));
        gdt_init();
        dprintf("%{mm:} GDT initialized\n", LIGHTBLUE);  
//        dprintf("mm_start.next: 0x%x\n", mm_start->next);
//        dprintf("mm_start.prev: 0x%x\n", mm_start->prev);
//        dprintf("mm_start.name: %s\n", mm_start->name);
//        dprintf("mm_start.size: %d\n", mm_start->size);
//        dprintf("mm_end.next: 0x%x\n", mm_end->next);
//        dprintf("mm_end.prev: 0x%x\n", mm_end->prev);
//        dprintf("mm_end.name: %s\n", mm_end->name);
//        dprintf("mm_end.size: %d\n", mm_end->size);
        
        dprintf("%{mm:} memory 0x%x - 0x%x (%dmb)\n",
                        LIGHTBLUE, (uint32)kernel_heap->start, (uint32)kernel_heap->end + sizeof(mm_header), ((uint32)kernel_heap->end + sizeof(mm_header) - (uint32)kernel_heap->start) / (1024 * 1024));
}

/**
 * Loads a page-directory to the CR3 register.
 * 
 * @param new the page-directory that shall be loaded to CR3
 */
void switch_page_dir(page_directory_t* new)
{
        current_dir = new;
        write_to_cr3((uint32)&new->tablesPhysical);
        uint32 cr0 = read_from_cr0();
        // set paging bit
        cr0 |= 0x80000000;
        write_to_cr0(cr0);
}        

/**
 * Returns a pointer to the required page.
 * 
 * @param address
 * @param make if make == 1 and the page doesn't already exist, it is created
 * @param dir the page-directory
 */
page_t *get_page(uint32 address, int make, page_directory_t *dir)
{
        address /= 0x1000;
        // Find the page table containing this address.
        uint32 table_index = address / 1024;
        if (dir->tables[table_index]) {
            return &dir->tables[table_index]->pages[address%1024];
        } else if(make) {
            uint32 tmp;
            dir->tables[table_index] = (page_table_t*)kmalloc(sizeof(page_table_t), 1, &tmp);
            memset(dir->tables[table_index], 0, 0x1000);
            dir->tablesPhysical[table_index] = tmp | 0x7; // PRESENT, RW, US.
            return &dir->tables[table_index]->pages[address%1024];
        } else {
            return 0;
        }
}        

/**
 * Allocates a fixed-size block of memory.
 * 
 * @param size the size of the block taht shall be allocated
 * @param aligned 1: the block shall be page-aligned
 * 
 * @return adress of the allocated block
 */
uint32 kmalloc(uint32 size, int page_aligned, uint32* phys)
{
        if (page_aligned == 1 && ((placement_addr & 0xFFFFF000) != placement_addr)) {
                placement_addr &= 0xFFFFF000;
                placement_addr += 0x1000;
        }
        //printf("\n\nplacement_addr: 0x%x\n\n",placement_addr);
        if (phys) {
                *phys = placement_addr;
        }
        uint32 tmp = placement_addr;
        placement_addr += size;
        return tmp;
}
