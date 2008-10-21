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
 * all functions concerning the allocation and deallocation of frames
 *
 * @author Johannes Schamburger
 * @author $LastChangedBy$
 * @version $Rev$
 */

#include "../include/types.h"
#include "../include/stdio.h"
#include "../include/string.h"
#include "../include/init.h"
#include "mm_paging.h"
#include "mm_bitmap.h"

/**
 * Marks a frame as occupied.
 * 
 * @param frame_addr address of the frame
 */
void set_frame(uint32 frame_addr)
{
        uint32 frame = frame_addr / 0x1000;
        frames[frame/32] |= (0x1 << (frame%32));
        //printf("frame_addr: 0x%x frame: %d -> frames[%d] := 0x%x (frame%%32: %d)\n",frame_addr,frame,frame/16,frames[frame/32],frame%32);
}

/**
 * Marks a frame as free.
 * 
 * @param frame_addr address of the frame
 */
void clear_frame(uint32 frame_addr)
{
        uint32 frame = frame_addr / 0x1000;
        frames[frame/32] &= ~(0x1 << (frame%32));
}

/**
 * Tests if a frame is free.
 * 
 * @param frame_addr address of the frame
 * @return 0: frame is occupied; any other value: frame is free
 */
bool test_frame(uint32 frame_addr)
{
        uint32 frame = frame_addr / 0x1000;
        if((frames[frame/32] & (0x1 << (frame%32))) != 0) {
                return 1;
        }
        return 0;
}

/**
 * Finds the first free frame in the bitset.
 * 
 * @return number of the first free frame
 */
uint32 first_free_frame()
{
        uint32 i, j;
        for (i = 0; i < nframes/32; i++) {
                if (frames[i] != 0xFFFFFFFF) { // nothing free -> next
                        for (j = 0; j < 32; j++) {
                                uint32 toTest = 0x1 << j;
                                if (!(frames[i] & toTest)) {
                                        return i*32+j;
                                }
                        }
                }
        }
        return -1;
}

/** 
 * Allocates a frame.
 * 
 * @param page          the page
 * @param kernel        1: frame is only accessible from the kernel
 * @param writable      1: frame is writable; 0: frame is read-only
 */
void alloc_frame(page_t *page, int kernel, int writeable)
{
        // Frame was already allocated -> quit
        if (page->frame != 0) {
                return;
        } else {
                uint32 first = first_free_frame();
                if (first == (uint32)-1) {
                        panic("There are no free frames!");
                }
                set_frame(first*0x1000);
                page->present = 1;
                page->rw = (writeable)?1:0;
                page->user = (kernel)?0:1;
                page->frame = first;
        }
}

/**
 * Deallocates a frame.
 * 
 * @param page the page
 */
void free_frame(page_t *page)
{
        uint32 frame;
        if (!(frame=page->frame)) { // The given page doesn't have an allocated frame!
                return;
        } else {
                clear_frame(frame);
                page->frame = 0x0;
        }
}

/**
 * just some tests to see if the bitset is working properly
 */
void bitset_test()
{
        uint32 i = 0;
        uint32 j;
        uint32 *ptr;
        
        uint32 first;
        /*set_frame(0x1002);
        set_frame(0x2005);
        set_frame(0x3010);
        set_frame(0x5000);
        set_frame(0x10032);
        set_frame(0x1F999);
        set_frame(0x20000);
        set_frame(0x22000);
        clear_frame(0x22000);*/
        printf("\n\nallocated frames: \n");
        
        for(j = 0; j <= nframes; j++) {
                if(test_frame(j * 0x1000)) {
                        printf("%d ", j);
                }
        }
        first = first_free_frame();
        printf("\nfirst free frame: 0x%x\n", first);
        
        bool test = test_frame(first * 0x1000);
        printf("test_frame(first): %b\n",test);
        if(test) {
                printf("frame first is set!\n");
        }
        set_frame(first * 0x1000);
        test = test_frame(first * 0x1000);
        printf("test_frame(first): %b\n",test);
        if(test_frame(first * 0x1000)) {
                printf("frame first is set!\n");
        }
        
        // set all frames as occupied
        /*for(uint32 f = 0; f < nframes; f++) {
                set_frame(0x1000*f);
        }
        first = first_free_frame();
        
        printf("\n\nbitset: \n");
        
        j = 0;
        for(ptr = frames; j <= 5; ptr++) {
                printf("%b",*ptr);
                j++;
                printf("\n");
        }
        
        printf("\nfirst free frame: 0x%x (nframes: 0x%x)\n", first, nframes);
        if(first == -1) {
                printf("no free frames");
        }
        page_t page;
        alloc_frame(page,0,0);*/

}
