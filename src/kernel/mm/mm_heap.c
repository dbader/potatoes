/* $Id$
 ********************************************************************************
 * _____   ____ _______    _______ ____  ______  _____                          *
 *|  __ \ / __ \__   __|/\|__   __/ __ \|  ____|/ ____|          Copyright 2008 *
 *| |__) | |  | | | |  /  \  | | | |  | | |__  | (___              Daniel Bader *
 *|  ___/| |  | | | | / /\ \ | | | |  | |  __|  \___ \           Vincenz Doelle *
 *| |    | |__| | | |/ ____ \| | | |__| | |____ ____) |    Johannes Schamburger *
 *|_|     \____/  |_/_/    \_\_|  \____/|______|_____/          Dmitriy Traytel *
 *                                                                              *
 *      Practical Oriented TeAching Tool, Operating (and) Educating System      *
 *                                                                              *
 *                           www.potatoes-project.tk                            *
 *******************************************************************************/

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
heap_t* create_heap(uint32 start_addr, uint32 end_addr, uint32 max_addr, uint8 supervisor, uint8 readonly,
                uint32 heap_addr)
                {
        heap_t *kheap = (heap_t*) heap_addr;
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
 * Returns the total size of a heap.
 *
 * @param heap  the heap
 * @return      size of the heap
 */
uint32 heap_get_size(heap_t *heap)
{
        return (uint32) heap->end + sizeof(mm_header) - (uint32) heap->start + 1;
}

/**
 * Expands 'heap' to 'new_size'.
 *
 * @param new_size      new size of the heap
 * @param heap          the heap that shall be expanded
 */
void heap_expand(uint32 new_size, heap_t *heap)
{

        uint32 old_size = heap_get_size(heap);
        // Sanity check.
        ASSERT(new_size > old_size);
        // Get the nearest following page boundary.
        if (new_size & 0xFFFFF000 != 0) {
                new_size &= 0xFFFFF000;
                new_size += 0x1000;
        }
        //dprintf("expanding heap to 0x%x\n", new_size);
        //dprintf("placement_addr: 0x%x\n", placement_addr);
        // Make sure we are not overreaching ourselves.
        ASSERT((uint32)heap->start + new_size <= heap->max_addr);

        // This should always be on a page boundary.

        uint32 i = old_size;
        while (i <= new_size) {
                alloc_frame(get_page((uint32) heap->start + i, 1, kernel_dir), (heap->supervisor) ? 1 : 0,
                                (heap->readonly) ? 0 : 1);
                i += 0x1000;
        }

        uint32 end_address = (uint32) heap->start + new_size;

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
uint32 heap_contract(uint32 new_size, heap_t *heap)
{

        uint32 old_size = heap_get_size(heap);
        // Sanity check.
        ASSERT(new_size < old_size);

        // Get the nearest following page boundary.
        if (new_size & 0x1000) {
                new_size &= 0x1000;
                new_size += 0x1000;
        }
        // Don't contract too far!
        if (new_size < HEAP_MIN_SIZE) {
                new_size = HEAP_MIN_SIZE;
        }

        uint32 i = old_size - 0x1000;
        while (new_size < i) {
                free_frame(get_page((uint32) heap->start + i, 0, kernel_dir));
                i -= 0x1000;
        }

        uint32 end_address = (uint32) heap->start + new_size;

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

/**
 * Sets up a new block in the heap.
 *
 * @param next_block    pointer to the block after the one to be inserted
 * @param position      end of the block before the one to be inserted
 * @param size          size of the new block
 * @param name          name of the new block
 *
 * @return              the header of the new block
 */
mm_header* heap_setup_block(mm_header* next_block, uint32 position, size_t size, char* name)
                {
        mm_header *new_header = (mm_header*) (position);

        // set the properties of the new block
        new_header->prev = next_block->prev;
        new_header->next = next_block;
        strncpy(new_header->name, name, sizeof(new_header->name) - 1);
        new_header->name[sizeof(new_header->name) - 1] = '\0';
        new_header->size = size;

        // insert the new block into the linked list
        next_block->prev->next = new_header;
        next_block->prev = new_header;

        return new_header;
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
void* heap_mallocn(size_t size, char *name, uint8 page_aligned, heap_t *heap)
                {
#ifdef MEM_FAILSAFE
        void *ret = mem;
        mem += size;
        return ret;
#endif
        //dprintf("allocating %d bytes; name: %s", size, name);
        mm_header *current_block;
        mm_header *new_header;
        uint32 position;
        //dprintf("kernel_heap->start: 0x%x; kernel_heap->end: 0x%x\n", (uint32)kernel_heap->start, (uint32)kernel_heap->end);
        //dprintf("kernel_heap->start->next: 0x%x; kernel_heap->end->next: 0x%x\n", (uint32)kernel_heap->start->next, (uint32)kernel_heap->end->next);

        /********************************************FIRST FIT********************************************* /
        // we loop through the whole list of allocated blocks and search for a hole which is big enough
        for (current_block = heap->start->next; current_block != heap->end->next; current_block = current_block->next) {

                // compute the beginning of the current hole
                position = ((uint32) (current_block->prev) + sizeof(mm_header) + (current_block->prev)->size);

                // is the current hole big enough?
                if ((uint32) current_block - position >= (size + sizeof(mm_header))) {

                        // setup the new block
                        new_header = heap_setup_block(current_block, position, size, name);

                        // we need to return a void-pointer pointing to the start of the block (and not of the header)
                        return (void*) ((uint32) new_header + sizeof(mm_header));
                }
        }
        /**************************************************************************************************/
        /***************************************WORST FIT**************************************************/
        mm_header *tmp_header = NULL;
        uint32 biggest_hole_size = 0;

        // we loop through the whole list of allocated blocks and search for the biggest hole
        for (current_block = kernel_heap->start->next; current_block != kernel_heap->end->next; current_block = current_block->next) {

                // compute the beginning of the current hole
                position = ((uint32) (current_block->prev) + sizeof(mm_header) + (current_block->prev)->size);

                // see if the current hole is the biggest one (so far) that fits
                if ((uint32) current_block - position >= (size + sizeof(mm_header)) && (uint32) current_block - position > biggest_hole_size) {
                        tmp_header = current_block;
                        biggest_hole_size = (uint32) current_block - position;
                }
        }

        if (tmp_header != NULL) {
                position = ((uint32) (tmp_header->prev) + sizeof(mm_header) + (tmp_header->prev)->size);
                // the random offset - not reasonable but nice for visualisation :-)
                // + rand() % (biggest_hole_size - (size + sizeof(mm_header)));

                // setup the new block
                new_header = heap_setup_block(tmp_header, position, size, name);

                // we need to return a void-pointer pointing to the start of the block (and not of the header)
                return (void*) ((uint32) new_header + sizeof(mm_header));
        }
        /**************************************************************************************************/
        /*********************************BEST FIT********************************************************** /
                mm_header *tmp_header = NULL;
                uint32 smallest_hole_size = heap_get_size(heap);

                // we loop through the whole list of allocated blocks and search for the biggest hole
                for (current_block = kernel_heap->start->next; current_block != kernel_heap->end->next; current_block = current_block->next) {

                        // compute the beginning of the current hole
                        position = ((uint32) (current_block->prev) + sizeof(mm_header) + (current_block->prev)->size);

                        // see if the current hole is the smallest one (so far) that fits
                        if ((uint32) current_block - position >= (size + sizeof(mm_header)) && (uint32) current_block - position < smallest_hole_size) {
                                tmp_header = current_block;
                                smallest_hole_size = (uint32) current_block - position;
                        }
                }

                if (tmp_header != NULL) {
                        position = ((uint32) (tmp_header->prev) + sizeof(mm_header) + (tmp_header->prev)->size);

                        // setup the new block
                        new_header = heap_setup_block(tmp_header, position, size, name);

                        // we need to return a void-pointer pointing to the start of the block (and not of the header)
                        return (void*) ((uint32) new_header + sizeof(mm_header));
                }
                 /**************************************************************************************************/
        // we found no hole that is big enough -> the heap needs to be expanded
        uint32 old_size = heap_get_size(heap);
        heap_expand(old_size + (size / (HEAP_EXPAND_STEP_SIZE) + 1) * HEAP_EXPAND_STEP_SIZE, heap);

        uint32 new_size = heap_get_size(heap);

        // check if the heap has reached the memory limit
        if(new_size < old_size + size) {
                return (void*) NULL;
        }

        return heap_mallocn(size, name, page_aligned, heap);
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
        mm_header *this = (mm_header*) ((uint32) start - sizeof(mm_header));
        // check if there is a valid mm_header structure at start
        if ((this->prev)->next == this) {
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
                _printf("%d bytes #{LBL}\"%s\"## at 0x%x\n", ptr->size, ptr->name, (uint32) ptr + sizeof(mm_header));
                total_blocks++;
                total_bytes += ptr->size;
        }

        _printf("#{LBL}Total %d blocks (%d bytes)##\n", total_blocks, total_bytes);
}
