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
 * Definitions for the functions and variables used in the memory management.
 *
 * @author Johannes Schamburger
 * @author $LastChangedBy$
 * @version $Rev$
 */

#include "../include/types.h"

#ifndef __MM_H_
#define __MM_H_

/**
 * the structure of a header of an occupied memory block
 */
typedef struct mm_header {
        struct mm_header *prev;         // pointer to the header of the previous occupied block
        struct mm_header *next;         // pointer to the header of the next occupied block
        char name[32];                  // name of the block
        uint32 size;                    // size of the current block
        // uint8 pad[100];
} mm_header;

#define KHEAP_START             0xC0000000
#define KHEAP_INITIAL_SIZE      0x500000
#define HEAP_MIN_SIZE           0x70000
/**
 * the heap is expanded only by steps of a certain size.
 */
#define HEAP_EXPAND_STEP_SIZE   1048576 // 1 MB

typedef struct
{
   mm_header *start;
   mm_header *end;
   uint32 max_addr;
   uint8 supervisor;
   uint8 readonly;
} heap_t;

void mm_init();
void mm_init_output();

heap_t* create_heap(uint32 start_addr, uint32 end_addr, uint32 max_addr, uint8 supervisor, uint8 readonly, uint32 heap_addr);
void* heap_mallocn(size_t size, char *name, uint8 page_aligned, heap_t *heap);
void heap_free(void *ptr, heap_t *heap);
void heap_mem_dump();
heap_t *kernel_heap;

/**
 * The structure of one GDT entry.
 * access byte: 
 *    7   |6        5|       4       |3                                0|
 * present|ring (0-3)|descriptor type|segment type (1010:code;0010:data)|
 * granularity byte:
 *      7     |      6     |   5    |          4         |3                           0|
 * granularity|operand size|always 0|available (always 0)|segment length (bits 19 - 16)|
 *      granularity bit: 0 -> 1 byte; 1 -> 4 kbyte
 *      operand size: 0 -> 16bit; 1 -> 32bit
 */
struct gdt_entry { // 64 bit
        uint16 limit_low;       // lower 16 bits of the limit
        uint16 base_low;        // lower 16 bits of the base
        uint8  base_middle;     // middle 8 bits of the base
        uint8  access;          // access flags
        uint8  granularity;     
        uint8  base_high;       // higher 8 bits of the base
} __attribute__((packed));

/**
 * The structure of the GDT pointer which tells the processor where to find our GDT
 */
struct gdt_pointer {    // 48 bit
        uint16 limit;   // size of our GDT
        uint32 base;    // address of the first entry of our GDT
} __attribute__((packed));

struct gdt_entry gdt[3]; // Our GDT consisting of 3 entries (null, data and code)
struct gdt_pointer gp;

void gdt_init();

void mm_move_block(uint32 dest, mm_header *src);

#endif /* __MM_H_ */
