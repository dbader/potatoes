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
 * @author Johannes Schamburger
 * @author $LastChangedBy$
 * @version $Rev$
 */

#include "../include/types.h"
#include "../include/const.h"
#include "../include/stdio.h"
#include "../include/string.h"
#include "../include/debug.h"
#include "../mm/mm.h"

static long _seed = 1L;

void srand(unsigned int seed)
{
  _seed = (long) seed;
}

int rand()
{
  return (((_seed = _seed * 214013L + 2531011L) >> 16) & 0x7fff);
}

/**
 * allocates size bytes and additionally saves a name in the header of the block.
 * ATTENTION: if malloc fails (i.e. there is not enough free space), the return value is 
 * (void*) NULL. So this should always be tested!
 * 
 * @param size  how much space shall be allocated
 * @param name  the name of the memory block (mainly for debugging purposes)
 * @return      pointer to the allocated space
 */

//
// Enables a braindead memory "manager" for debugging purposes. (daniel)
// Use this to make sure a bug is not related to MM.
//
//#define MEM_FAILSAFE

#ifdef MEM_FAILSAFE
        void* mem = (void*) 0x200000; // assume this is past the kernel code...
#endif

void* mallocn(size_t size, char *name)
{
#ifdef MEM_FAILSAFE        
        void *ret = mem;
        mem += size;
        return ret;
#endif  
        
        //size += 100;
        
        mm_header *ptr;
        mm_header *new_header;
        uint32 end_of_prev;
        for(ptr = mm_start->next; ptr != mm_end->next; ptr = ptr->next) {
                end_of_prev = ((uint32)(ptr->prev) + sizeof(mm_header) + (ptr->prev)->size);
                if((uint32)ptr - end_of_prev >= (size + sizeof(mm_header))) {
                        
                        new_header = (mm_header*) (end_of_prev);
                        new_header->prev = ptr->prev;
                        new_header->next = ptr;
                        strncpy(new_header->name, name, sizeof(new_header->name) - 1);
                        new_header->size = size;
                        
                        ptr->prev->next = new_header;
                        ptr->prev = new_header;
                        
                        return (void*) ((uint32)new_header + sizeof(mm_header));
                }
        }
        
        return (void*) NULL;
}

/**
 * allocates size bytes cleaned and additionally saves a name in the header of the block
 * 
 * @param size  how much space shall be allocated
 * @param name  the name of the memory block (mainly for debugging purposes)
 * @return      pointer to the allocated space
 */

/*void* malloc_name(uint32 size, char* name)
{
//        printf("allocation of %d bytes.0x%x\n", size, mm_occupied_top);
        mm_occupied_top += size;
//        printf("mm_occupied_top neu: 0x%x\n",mm_occupied_top);
        if ((uint32)mm_occupied_top < (uint32)mm_end) {
                return (void*) (mm_occupied_top - size);
        } else {
                mm_occupied_top -= size;
                return (void*) NULL;
        }
}*/

/** 
 * allocates size bytes
 * ATTENTION: if malloc fails (i.e. there is not enough free space), the return value is 
 * (void*) NULL. So this should always be tested!
 * 
 * @param size  how much space shall be allocated
 * @return      pointer to the allocated space
 */ 
void* malloc(size_t size)
{
        return (void*) mallocn(size,"noname");
}

/**
 * allocates space for n elements of the same size and additionally saves a name in 
 * the header of the block
 * 
 * @param n     number of elements
 * @param size  size of each element
 */
void* callocn(size_t n, size_t size, char *name)
{
        void* ret = mallocn(n * size, name);
        if(ret == (void*) NULL) {
                return (void*) NULL;
        }
        mm_header* header = (mm_header*) ((uint32)ret - sizeof(mm_header));
        bzero(ret, n * size);
        return ret;
}

/**
 * allocates space for n elements of the same size
 * 
 * @param n     number of elements
 * @param size  size of each element
 */
void* calloc(size_t n, size_t size) 
{
        return callocn(n, size, "noname");
}

void mem_dump() 
{
        mm_header *ptr;
        int total_bytes = 0;
        int total_blocks = 0;
        for(ptr = mm_start->next; ptr != mm_end->next; ptr = ptr->next) {
                dprintf("%d bytes \"%s\" at 0x%x\n", ptr->size, ptr->name, (uint32)ptr + sizeof(mm_header));
                total_blocks++;
                total_bytes += ptr->size;
        }
        
        dprintf("Total %d blocks (%d bytes)\n", total_blocks, total_bytes);
}

/**
 * frees a memory block
 * 
 * @param start pointer to the start of the block that shall be freed
 */
void free(void *start)
{
#ifdef MEM_FAILSAFE
       return;
#endif
        //dprintf("free 0x%x\n", start);
        mm_header *this = (mm_header*) ((uint32)start - sizeof(mm_header));
        // check if there is a valid mm_header structure at start
        if((this->prev)->next == this) {
                (this->prev)->next = this->next;
                (this->next)->prev = this->prev;
                return;
        }
        dprintf("ERROR: free(): attempt to free unallocated block 0x%x\n", start);
}

/**
 * reallocates a memory block to size bytes
 * ATTENTION: if realloc fails (i.e. there is not enough free space), the return value is 
 * (void*) NULL. So this should always be tested! 
 * Especially realloc() shouldn't be used like this:
 * 
 * int* p = malloc(50);
 * p = realloc(100);
 * 
 * In this case, if realloc fails, p is overwritten by (void*) NULL. So, the memory allocated 
 * for p is no longer accessible, which means that the data stored in p is lost and the memory
 * allocated for p can't be used any more. 
 * 
 * @param pointer       pointer to the old allocated space
 * @param size          the new size
 * @return              pointer to the reallocated space
 */
void* realloc(void *pointer, size_t size)
{
        mm_header* hdr = (mm_header*)(pointer - sizeof(mm_header));
        // the free space after pointer is big enough for the new size 
        if((uint32)hdr->next >= (uint32)((uint32) pointer + size)) {
                hdr->size = size;
                return pointer;
        }
        void* new = mallocn(size, hdr->name);
        if(new == (void*) NULL) {
                return (void*) NULL;
        }
        memmove(new, pointer, hdr->size);
        free(pointer);
        return new;
}      

/**
 * function to return the free memory space
 * 
 * @return      free memory space in bytes
 */  
uint32 free_memory()
{
        mm_header *ptr;
        uint32 free = 0;
        for(ptr = mm_start->next; ptr != mm_end->next; ptr = (*ptr).next) {
                free += (uint32)ptr - ((uint32)ptr->prev + sizeof(mm_header) + (ptr->prev)->size);
        }
        
        return free;
}
