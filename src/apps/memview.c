/* $Id:  $
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
 * Main source file of the memory viewer tool "memview".
 * "memview" graphically displays the POTATOES memory layout. It can be used
 * to visualize the effects of different memory allocation strategies.
 *
 * Example:
 *      - start memview
 *      - allocate some blocks by pressing 'a'
 *      - now free some blocks via 'f'
 *      - allocate some more blocks again
 *      -> you will see that the memory manager uses a first fit strategy
 *         because freed spaces are filled again from 'left to right'.
 *
 * Every function and variable in this file should be prefixed with 'mv' (for 'memview')
 * to avoid cluttering the global namespace and nasty errors resulting of that.
 *
 * @author Daniel Bader
 * @author $LastChangedBy:  $
 * @version $Rev: 12 $
 */
#include "../kernel/pm/syscalls_cli.h"
#include "../kernel/include/stdio.h"
#include "../kernel/include/string.h"
#include "../kernel/include/const.h"
#include "../kernel/io/io.h"
#include "../kernel/pm/pm_main.h"

#include "../kernel/mm/mm.h"

#include "games.h" /* for the key constants */
//#include "memview.h"

extern bool keydown(char key, int fd);

/** The maximum amount of dummy blocks that can be allocated. */
#define MEMVIEW_MAX_DUMMY_BLOCKS 100

/** Keeps track of all dummy memory blocks we allocate. */
void* mv_allocated_blocks[MEMVIEW_MAX_DUMMY_BLOCKS];

/** Display backbuffer */
uint8 mv_disp[25 * 80];

/** /dev/framebuffer file descriptor */
int mv_framebuf;

/** Size of the kernel heap. Careful, this changes all the
 *  time as the heap grows and contracts as needed. */
unsigned int mv_total_mem;

/** The 'resolution' of one visual block (ie one pixel in
 *  the graphical memory view). */
unsigned int mv_bytes_per_block;

/**
 * Finds a free slot for a new dummy block. Returns -1 if all slots are occupied.
 *
 * @return index of the free slot or -1 if no free slot could be found.
 */
int get_free_dummy_slot()
{
        for (int i=0; i<MEMVIEW_MAX_DUMMY_BLOCKS; i++) {
                if (mv_allocated_blocks[i] == 0) {
                        return i;
                }
        }

        return -1;
}

/**
 * Releases all dummy blocks we allocated.
 */
void free_dummy_blocks()
{
        for (int i=0; i<MEMVIEW_MAX_DUMMY_BLOCKS; i++) {
                if (mv_allocated_blocks[i] != 0) {
                        _free(mv_allocated_blocks[i]);
                }
        }
}

/**
 * Allocates a new dummy memory block of the specified size.
 *
 * @param size size of the block in bytes
 * @return TRUE if a block of the given size was allocated, FALSE if not
 */
bool allocate_dummy_block(uint32 size)
{
        int alloc_slot = get_free_dummy_slot();
        if (alloc_slot >= 0) {
                mv_allocated_blocks[alloc_slot] = _malloc(size);
        }
}

/**
 * Frees the first dummy block found in allocated_blocks.
 *
 * @return TRUE if a block was released, FALSE if no active blocks were found
 */
bool free_dummy_block()
{
        for (int i=0; i<MEMVIEW_MAX_DUMMY_BLOCKS; i++) {
                if (mv_allocated_blocks[i] != 0) {
                        _free(mv_allocated_blocks[i]);
                        mv_allocated_blocks[i] = NULL;
                        return TRUE;
                }
        }

        return FALSE;
}

/**
 * Mark all 'visual' blocks that lie within the memory block at address 'start' with length 'size'.
 *
 * @param start the starting address of the block
 * @param size the size of the block in bytes
 */
void mark_visual_block(uint32 start, uint32 size)
{
        uint32 block_pos = (start - (uint32)kernel_heap->start) / mv_bytes_per_block;

        while (block_pos < sizeof(mv_disp)) {
                if (size < mv_bytes_per_block) {
                        /* Visual blocks which are not completely filled by the first allocated
                         * block 'inside them' are marked yellow. Note that this does not necessarily
                         * mean the block is not completely occupied. Maybe we should also mark
                         * these blocks red. Yet somehow, I feel they are special :) */
                        mv_disp[block_pos] = YELLOW;
                        break;
                } else {
                        mv_disp[block_pos] = RED;
                        size -= mv_bytes_per_block;
                }

                block_pos++;
        }
}

/**
 * Updates the memview tool display. We iterate through all memory manager block descriptors
 * and mark affected 'visual' blocks accordingly. This is a bit of a hack as we access
 * internal structures directly.
 */
void update_view()
{
        memset(mv_disp, GREEN, sizeof(mv_disp));

        mm_header *block;
        for (block = kernel_heap->start->next; block != kernel_heap->end->next; block = block->next) {
                mark_visual_block((uint32)block, block->size);
        }

        _write(mv_framebuf, mv_disp, sizeof(mv_disp));
}

/**
 * The memory viewer tool entry function.
 */
void memview_main(void)
{
        memset(mv_allocated_blocks, 0, sizeof(mv_allocated_blocks));

        int keyboard = _open("/dev/keyboard", 0, 0);
        int stdin = _open("/dev/stdin", 0, 0);

        // Determine initial kernel heap size and depending on that a reasonable visual block size
        mv_total_mem = (uint32)kernel_heap->end + sizeof(mm_header) - (uint32)kernel_heap->start;
        mv_bytes_per_block = mv_total_mem / (25*80);

        _printf("Welcome to #{YEL}memview##, the #{BLU}POTATOES## #{GRE}visual memory viewer## tool!\n\n");
        _printf("Kernel heap from %x to %x\n", (uint32)kernel_heap->start, (uint32)kernel_heap->end + sizeof(mm_header));
        _printf("%dkb heap size\nNote: the kernel heap size changes as the heap expands and contracts\n", mv_total_mem / 1024);
        _printf("Initial resolution: 1 block = %dkb\n\n", mv_bytes_per_block / 1024);
        _printf("\n#{YEL}Usage:##\nu - update view\na - allocate block\nf - free block\ni - zoom in\no - zoom out\nESCAPE - exit memview\n\n");
        _printf("#{YEL}Block types:##\n#{GRE}### - completely free region\n#{YEL}### - only partly occupied by first allocation\n#{RED}### - completely filled region\n\n[Press any key to start]\n");

        // Wait for keypress to display welcome message and usage.
        _fgetch(stdin);

        mv_framebuf = _open("/dev/framebuffer", 0, 0);
        update_view();

        while (!keydown(ESCAPE, keyboard)) {
                switch (_fgetch(stdin)) {
                case 'a': // allocate a block of the current visual block size
                        /* NOTE: Maybe this should allocate a block of random length to
                         *       better visualize different memory manager strategies.
                         *       We will have to play around with that as soon as we
                         *       have new mm strategies :-)
                         */
                        allocate_dummy_block(mv_bytes_per_block);
                        update_view();
                        break;

                case 'f': // free one block
                        free_dummy_block();
                        update_view();
                        break;

                case 'i': // zoom in
                        mv_bytes_per_block -= 512;
                        update_view();
                        break;

                case 'o': // zoom out
                        mv_bytes_per_block += 512;
                        update_view();
                        break;

                case 'u': // update view
                        update_view();
                        break;
                }

                halt();
        }

        _close(mv_framebuf);
        free_dummy_blocks();

        _close(stdin);
        _close(keyboard);
        _exit(0);
}

/**
 * Shell command wrapper for the memview tool
 */
void shell_cmd_memview(int argc, char *argv[])
{
        pm_create_thread("memview", memview_main, 4096);
}
