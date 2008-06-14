/* $Id: malloc_free.c 64 2008-05-16 18:17:51Z jschamburger $
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
 * @author $LastChangedBy: jschamburger $
 * @version $Rev: 64 $
 */

#include "../include/types.h"
#include "../include/const.h"
#include "../include/stdio.h"
#include "../include/string.h"
#include "../mm/mm.h"

/**
 * allocates size bytes and additionally saves a name in the header of the block.
 * ATTENTION: if malloc fails (i.e. there is not enough free space), the return value is 
 * (void*) NULL. So this should always be tested!
 * 
 * @param size  how much space shall be allocated
 * @param name  the name of the memory block (mainly for debugging purposes)
 * @return      pointer to the allocated space
 */

void* mallocn(size_t size, char *name)
{
        // search for a free block big enough: we start at the first block after mm_start, we stop after mm_end
        mm_header *ptr;
        mm_header *new_header;
        for(ptr = mm_start->next; ptr != mm_end->next; ptr = (*ptr).next) {
                // test if the free memory between the current and the previous block is big enough 
                // (attention: the header of the previous block and the header of the new block must be considered)
                if((uint32)ptr - ((uint32)(ptr->prev) + (uint32)sizeof(mm_header) + (ptr->prev)->size)
                         >= (size + (uint32)sizeof(mm_header))) {
                        // set header for the new block, update the header of the start block and return the adress of the block (after the header!)
                        new_header = (mm_header*) ((uint32)(ptr->prev) + 
                                        (uint32)sizeof(mm_header) + (ptr->prev)->size);
                        new_header->prev = (*ptr).prev;
                        new_header->next = ptr;
                        new_header->name = name;
                        new_header->size = size;
                        
                        (ptr->prev)->next = new_header;
                        ptr->prev = new_header;
                        
                        return (void*) ((uint32)new_header + (uint32)sizeof(mm_header));
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

void* mallocn_clean(size_t size, char *name)
{
        void *p = mallocn(size, name);
        bzero(p, size);
        return p;
}

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
 * allocates size bytes cleaned
 * 
 * @param size  how much space shall be allocated
 * @return      pointer to the allocated space
 */ 
void* malloc_clean(size_t size)
{
        void *p = malloc(size);
        bzero(p, size);
        return p;
}

/**
 * frees a memory block
 * 
 * @param start pointer to the start of the block that shall be freed
 */
void free(void *start)
{
        mm_header *this = (mm_header*) ((uint32)start - sizeof(mm_header));
        // element is removed from the list (the allocated memory is not touched in any way)
        (this->prev)->next = this->next;
        (this->next)->prev = this->prev;
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
                printf("new size: 0x%x\nnext: 0x%x\nprev: 0x%x",hdr->size, (uint32)hdr->next, (uint32)hdr->prev);
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

