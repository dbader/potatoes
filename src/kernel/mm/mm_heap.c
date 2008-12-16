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
#include "../include/string.h"
#include "../include/stdio.h"
#include "../include/const.h"
#include "../include/debug.h"
#include "../include/assert.h"
#include "../include/stdlib.h"
#include "mm.h"
#include "mm_bitmap.h"
#include "mm_paging.h"

/**
 * Creates a heap.
 *
 * @param start_addr    virtual start address of the heap
 * @param end_addr      virtual end address of the heap
 * @param max_addr      virtual maximum address of the heap (needed for expand)
 * @param supervisor    1: supervisor mode; 0: user mode
 * @param readonly      1: read-only; 0: readable and writeable
 * @param heap_addr     physical address of the heap structure
 * @return              the created heap
 */
heap_t* create_heap(uint32 start_addr, uint32 end_addr, uint32 max_addr, uint8 supervisor, uint8 readonly, uint32 heap_addr)
{
        heap_t *kheap = (heap_t*)heap_addr;
        //dprintf("heap position: 0x%x\n", (uint32)kheap);
        // All our assumptions are made on startAddress and endAddress being page-aligned.

        // Make sure the start address is page-aligned.
        if (start_addr & 0xFFFFF000 != 0) {
                start_addr &= 0xFFFFF000;
                start_addr += 0x1000;
        }

        // set the start and the end of the memory
        mm_header *start_header = (mm_header*) start_addr;
        mm_header *end_header = (mm_header*) (end_addr - sizeof(mm_header));

        // set the headers for mm_start and mm_end (both have size = 0; just needed for linked list stuff)
        start_header->next = end_header;
        start_header->prev = end_header;
        strncpy(start_header->name, "start", sizeof(start_header->name) - 1);
        start_header->size = 0;

        end_header->next = start_header;
        end_header->prev = start_header;
        strncpy(end_header->name, "end", sizeof(end_header->name) - 1);
        end_header->size = 0;

        //dprintf("start: 0x%x\n", (uint32)start_header);
        //dprintf("start.next: 0x%x\n", start_header->next);
        //dprintf("start.prev: 0x%x\n", start_header->prev);
        //dprintf("start.name: %s\n", start_header->name);
        //dprintf("start.size: %d\n", start_header->size);
//        dprintf("end: 0x%x\n", (uint32)end_header);
//        dprintf("end.next: 0x%x\n", end_header->next);
//        dprintf("end.prev: 0x%x\n", end_header->prev);
//        dprintf("end.name: %s\n", end_header->name);
//        dprintf("end.size: %d\n", end_header->size);

        kernel_heap->start = start_header;
        kernel_heap->end = end_header;
        kernel_heap->max_addr = max_addr;
        kernel_heap->supervisor = supervisor;
        kernel_heap->readonly = readonly;

        return kernel_heap;
}

/**
 * Expands 'heap' to 'new_size'.
 *
 * @param new_size      new size of the heap
 * @param heap          the heap that shall be expanded
 */
void expand(uint32 new_size, heap_t *heap)
{

        // Sanity check.
        ASSERT(new_size > (uint32)heap->end + sizeof(mm_header) - (uint32)heap->start);
        // Get the nearest following page boundary.
        if (new_size&0xFFFFF000 != 0) {
                new_size &= 0xFFFFF000;
                new_size += 0x1000;
        }
        //dprintf("expanding heap to 0x%x\n", new_size);
        //dprintf("placement_addr: 0x%x\n", placement_addr);
        // Make sure we are not overreaching ourselves.
        ASSERT((uint32)heap->start + new_size <= heap->max_addr);

        // This should always be on a page boundary.
        uint32 old_size = (uint32)heap->end + sizeof(mm_header) - (uint32)heap->start;
        uint32 i = old_size;
        while (i <= new_size) {
                alloc_frame( get_page((uint32)heap->start+i, 1, kernel_dir),(heap->supervisor)?1:0, (heap->readonly)?0:1);
                i += 0x1000;
        }

        uint32 end_address = (uint32)heap->start + new_size;

        //dprintf("end_address: 0x%x\n", end_address);
        // modify the headers for heap->start and heap->end
        mm_header* end_header = (mm_header*) (end_address - sizeof(mm_header));
        mm_header* tmp = heap->end->prev;

        tmp->next = end_header;
        heap->end = end_header;
        heap->end->next = heap->start;
        heap->end->prev = tmp;
        strncpy(heap->end->name, "end", sizeof(heap->end->name) - 1);
        heap->end->size = 0;

        heap->start->prev = heap->end;
        heap->start->size = 0;

}

/**
 * Contracts 'heap' to 'new_size'.
 *
 * @param new_size      new size of the heap
 * @param heap          the heap that shall be contracted
 * @return              new size of the heap
 */
uint32 contract(uint32 new_size, heap_t *heap)
{
        // Sanity check.
        ASSERT(new_size < (uint32)heap->end + sizeof(mm_header) - (uint32)heap->start);

        // Get the nearest following page boundary.
        if (new_size&0x1000) {
                new_size &= 0x1000;
                new_size += 0x1000;
        }
        // Don't contract too far!
        if (new_size < HEAP_MIN_SIZE) {
                new_size = HEAP_MIN_SIZE;
        }
        uint32 old_size = (uint32)heap->end - (uint32)heap->start;
        uint32 i = old_size - 0x1000;
        while (new_size < i) {
                free_frame(get_page((uint32)heap->start + i, 0, kernel_dir));
                i -= 0x1000;
        }

        uint32 end_address = (uint32)heap->start + new_size;

        // modify the headers for heap->start and heap->end
        mm_header *end_header = (mm_header*) (end_address - sizeof(mm_header));
        heap->end = end_header;
        heap->end->next = heap->start;
        heap->end->prev = heap->start;
        strncpy(heap->end->name, "end", sizeof(heap->end->name) - 1);
        heap->end->size = 0;

        heap->start->next = heap->end;
        heap->start->prev = heap->end;
        heap->start->size = 0;

        return new_size;
}

//#define MEM_FAILSAFE

#ifdef MEM_FAILSAFE
        void* mem = (void*) 0x500000; // assume this is past the kernel code...
#endif
/**
 * Allocates 'size' bytes and additionally saves a name in the header of the block.
 *
 * @param size          how much space shall be allocated
 * @param name          name of the memory block (mainly for debugging purposes)
 * @param page_aligned  1: the block is allocated at a page-aligned address
 * @param heap          the heap in which the block shall be allocated
 * @return              pointer to the allocated space
 */
void* heap_mallocn(size_t size, char *name,  uint8 page_aligned, heap_t *heap)
{
#ifdef MEM_FAILSAFE
        void *ret = mem;
        mem += size;
        return ret;
#endif
        //dprintf("allocating %d bytes; name: %s", size, name);
        mm_header *ptr;
        mm_header *new_header;
        uint32 end_of_prev;
        //dprintf("kernel_heap->start: 0x%x; kernel_heap->end: 0x%x\n", (uint32)kernel_heap->start, (uint32)kernel_heap->end);
        //dprintf("kernel_heap->start->next: 0x%x; kernel_heap->end->next: 0x%x\n", (uint32)kernel_heap->start->next, (uint32)kernel_heap->end->next);
        for(ptr = kernel_heap->start->next; ptr != kernel_heap->end->next; ptr = ptr->next) {
                end_of_prev = ((uint32)(ptr->prev) + sizeof(mm_header) + (ptr->prev)->size);
                //dprintf("end_of_prev: 0x%x, ptr: 0x%x, diff: 0x%x\n", (uint32)end_of_prev, (uint32)ptr, (uint32)ptr - (uint32)end_of_prev);
                if((uint32)ptr - end_of_prev >= (size + sizeof(mm_header))) {

                        new_header = (mm_header*) (end_of_prev);
                        new_header->prev = ptr->prev;
                        new_header->next = ptr;
                        strncpy(new_header->name, name, sizeof(new_header->name) - 1);
                        new_header->name[sizeof(new_header->name) - 1] = '\0';
                        new_header->size = size;

                        ptr->prev->next = new_header;
                        ptr->prev = new_header;
                        //dprintf("return address: 0x%x\n", (uint32)new_header + sizeof(mm_header));

                        return (void*) ((uint32)new_header + sizeof(mm_header));
                }
        }

        uint32 old_size = (uint32)kernel_heap->end + sizeof(mm_header) - (uint32)kernel_heap->start + 1;
        expand(old_size + size, kernel_heap);
        return heap_mallocn(size, name, page_aligned, heap);

        return (void*) NULL;
}

/**
 * Frees a memory block.
 *
 * @param start pointer to the start of the block that shall be freed
 */
void heap_free(void *start, heap_t *heap)
{
#ifdef MEM_FAILSAFE
       return;
#endif
        mm_header *this = (mm_header*) ((uint32)start - sizeof(mm_header));
        // check if there is a valid mm_header structure at start
        if((this->prev)->next == this) {
                (this->prev)->next = this->next;
                (this->next)->prev = this->prev;
                return;
        }
        dprintf("ERROR: free(): attempt to free unallocated block 0x%x\n", start);
}

extern void _printf(char *fmt, ...);
/**
 * Dumps the heap.
 */
void heap_mem_dump()
{
        mm_header *ptr;
        int total_bytes = 0;
        int total_blocks = 0;
        for (ptr = kernel_heap->start->next; ptr != kernel_heap->end->next; ptr = ptr->next) {
                _printf("%d bytes #{LBL}\"%s\"## at 0x%x\n", ptr->size, ptr->name, (uint32)ptr + sizeof(mm_header));
                total_blocks++;
                total_bytes += ptr->size;
        }

        _printf("#{LBL}Total %d blocks (%d bytes)##\n", total_blocks, total_bytes);
}
