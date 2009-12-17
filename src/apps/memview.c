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
#include "games.h" /* for the KEY_... constants */

extern uint32 heap_get_size(heap_t *heap);
extern bool keydown(char key, int fd);

/** The maximum amount of dummy blocks that can be allocated. */
#define MEMVIEW_MAX_DUMMY_BLOCKS 10000

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
void free_all_dummy_blocks()
{
        for (int i=0; i<MEMVIEW_MAX_DUMMY_BLOCKS; i++) {
                if (mv_allocated_blocks[i] != 0) {
                        _free(mv_allocated_blocks[i]);
                        mv_allocated_blocks[i] = NULL;
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
        /* Use this to free the youngest block instead of the oldest: */
        // for (int i = MEMVIEW_MAX_DUMMY_BLOCKS-1; i >= 0; i--) {

        for (int i = 0; i < MEMVIEW_MAX_DUMMY_BLOCKS; i++) {
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
                         * block 'inside them' are marked pink. Note that this does not necessarily
                         * mean the block is not completely occupied. Maybe we should also mark
                         * these blocks red. Yet somehow, I feel they are special :)
                         *
                         * TODO: ideally the meaning of a pink block should be that the visual block is not
                         *       completely filled (even by multiple allocated blocks - so we would have to
                         *       keep track of that).*/
                        mv_disp[block_pos] = PINK;
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
        mv_total_mem = (uint32)kernel_heap->end + sizeof (mm_header) - (uint32)kernel_heap->start;

        memset(mv_disp, DARKGREY, sizeof(mv_disp));

        // Mark current maximum length of the kernel heap
        mv_total_mem = heap_get_size(kernel_heap);
        for (int i = 0; i < mv_total_mem / mv_bytes_per_block; i++) {
                mv_disp[i] = GREEN;
        }

        // Mark all allocated blocks
        mm_header *block;
        for (block = kernel_heap->start->next; block != kernel_heap->end->next; block = block->next) {
                mark_visual_block((uint32)block, block->size);
        }

        _write(mv_framebuf, mv_disp, sizeof(mv_disp));
}

/**
 * Disables the framebuffer and clears the textmode screen.
 */
void mv_switch_to_textmode()
{
        _close(mv_framebuf);

        for(int i = 0; i < 25; i++) {
                _printf("\n");
        }
}

/**
 * Enables the framebuffer.
 */
void mv_switch_to_graphicsmode()
{
        mv_framebuf = _open("/dev/framebuffer", 0, 0);
}

/**
 * Displays some memory statistics.
 */
void mv_show_stats()
{
        mm_header *ptr;
        int total_bytes = 0;
        int total_blocks = 0;
        int total_free_bytes = 0;
        int total_free_blocks = 0;
        int position;

        for (ptr = kernel_heap->start->next; ptr != kernel_heap->end->next; ptr = ptr->next) {
                total_blocks++;
                total_bytes += ptr->size;
                position = ((int) (ptr->prev) + sizeof(mm_header) + (ptr->prev)->size);
                if ((int)ptr!=position && ptr != kernel_heap->end) {
                        total_free_blocks++;
                        total_free_bytes += (int)ptr - position - sizeof(mm_header);
                }
        }

        _printf("#{YEL}MEMVIEW STATS##\n--------------------------------------------------------------------------------\n");

        _printf("Kernel heap from 0x%x to 0x%x\n", (uint32)kernel_heap->start, (uint32)kernel_heap->end + sizeof (mm_header));
        _printf("Heap size: %dkb (%d%% full)\n", mv_total_mem / 1024, (int)((float)total_bytes / (float)mv_total_mem * 100));
        _printf("Display resolution: 1 block = %d bytes\n\n", mv_bytes_per_block);
        _printf("%d bytes in %d blocks\n", total_bytes, total_blocks);
        _printf("Average block size: %d bytes\n", total_bytes / total_blocks);
        _printf("Average free block size: %d bytes\n", total_free_bytes / total_free_blocks);

        _fgetch(_open("/dev/stdin", 0, 0));
}

/**
 * Conducts a simple memory benchmark. FYI this was generated by a script... ;)
 */
void mv_do_benchmark()
{
        _printf("BENCHMARKING: ");

        free_all_dummy_blocks();

        allocate_dummy_block(17472);
        free_dummy_block();
        allocate_dummy_block(69382);
        free_dummy_block();
        allocate_dummy_block(20947);
        allocate_dummy_block(65895);
        free_dummy_block();
        allocate_dummy_block(43560);
        free_dummy_block();
        allocate_dummy_block(28139);
        free_dummy_block();
        allocate_dummy_block(17117);
        allocate_dummy_block(55948);
        allocate_dummy_block(17951);
        allocate_dummy_block(8243);
        free_dummy_block();
        _printf(".");
        allocate_dummy_block(24158);
        allocate_dummy_block(47781);
        allocate_dummy_block(15789);
        allocate_dummy_block(69879);
        free_dummy_block();
        allocate_dummy_block(40557);
        free_dummy_block();
        allocate_dummy_block(5569);
        free_dummy_block();
        allocate_dummy_block(65439);
        free_dummy_block();
        allocate_dummy_block(12880);
        free_dummy_block();
        allocate_dummy_block(60621);
        allocate_dummy_block(48204);
        _printf(".");
        allocate_dummy_block(52526);
        allocate_dummy_block(21711);
        free_dummy_block();
        allocate_dummy_block(62688);
        allocate_dummy_block(7254);
        allocate_dummy_block(4656);
        free_dummy_block();
        allocate_dummy_block(60099);
        free_dummy_block();
        allocate_dummy_block(37330);
        allocate_dummy_block(9118);
        allocate_dummy_block(39687);
        allocate_dummy_block(60146);
        free_dummy_block();
        _printf(".");
        allocate_dummy_block(22928);
        allocate_dummy_block(45517);
        allocate_dummy_block(60317);
        free_dummy_block();
        allocate_dummy_block(49194);
        free_dummy_block();
        allocate_dummy_block(31617);
        allocate_dummy_block(2595);
        allocate_dummy_block(50396);
        allocate_dummy_block(59238);
        allocate_dummy_block(21389);
        free_dummy_block();
        allocate_dummy_block(21303);
        _printf(".");
        allocate_dummy_block(43855);
        allocate_dummy_block(43437);
        free_dummy_block();
        allocate_dummy_block(53731);
        allocate_dummy_block(50861);
        free_dummy_block();
        allocate_dummy_block(4981);
        free_dummy_block();
        allocate_dummy_block(69027);
        free_dummy_block();
        allocate_dummy_block(9028);
        free_dummy_block();
        allocate_dummy_block(51008);
        allocate_dummy_block(14172);
        free_dummy_block();
        allocate_dummy_block(54422);
        free_dummy_block();
        _printf(".");
        allocate_dummy_block(53012);
        allocate_dummy_block(66210);
        free_dummy_block();
        allocate_dummy_block(17541);
        allocate_dummy_block(58399);
        free_dummy_block();
        allocate_dummy_block(66961);
        free_dummy_block();
        allocate_dummy_block(38290);
        free_dummy_block();
        allocate_dummy_block(4254);
        free_dummy_block();
        allocate_dummy_block(38766);
        allocate_dummy_block(50011);
        free_dummy_block();
        allocate_dummy_block(9156);
        _printf(".");
        allocate_dummy_block(62414);
        free_dummy_block();
        allocate_dummy_block(941);
        allocate_dummy_block(13312);
        allocate_dummy_block(20971);
        free_dummy_block();
        allocate_dummy_block(61601);
        allocate_dummy_block(57586);
        allocate_dummy_block(2635);
        free_dummy_block();
        allocate_dummy_block(4320);
        free_dummy_block();
        allocate_dummy_block(32261);
        free_dummy_block();
        allocate_dummy_block(29424);
        _printf(".");
        allocate_dummy_block(11554);
        allocate_dummy_block(67462);
        free_dummy_block();
        allocate_dummy_block(61240);
        allocate_dummy_block(40092);
        allocate_dummy_block(24957);
        free_dummy_block();
        allocate_dummy_block(25519);
        allocate_dummy_block(3448);
        free_dummy_block();
        allocate_dummy_block(21001);
        allocate_dummy_block(16907);
        allocate_dummy_block(3914);
        _printf(".");
        allocate_dummy_block(13526);
        free_dummy_block();
        allocate_dummy_block(60148);
        free_dummy_block();
        allocate_dummy_block(39712);
        allocate_dummy_block(46512);
        allocate_dummy_block(18333);
        allocate_dummy_block(12187);
        allocate_dummy_block(53490);
        allocate_dummy_block(57685);
        allocate_dummy_block(33760);
        free_dummy_block();
        allocate_dummy_block(40235);
        free_dummy_block();
        _printf(".");
        allocate_dummy_block(26018);
        free_dummy_block();
        allocate_dummy_block(46493);
        free_dummy_block();
        allocate_dummy_block(25334);
        free_dummy_block();
        allocate_dummy_block(43774);
        allocate_dummy_block(11781);
        free_dummy_block();
        allocate_dummy_block(48102);
        free_dummy_block();
        allocate_dummy_block(31576);
        free_dummy_block();
        allocate_dummy_block(42199);
        allocate_dummy_block(39590);
        allocate_dummy_block(62127);
        _printf(".");
        allocate_dummy_block(61081);
        free_dummy_block();
        allocate_dummy_block(34432);
        allocate_dummy_block(1325);
        free_dummy_block();
        allocate_dummy_block(19254);
        allocate_dummy_block(25880);
        allocate_dummy_block(12250);
        allocate_dummy_block(35156);
        allocate_dummy_block(507);
        allocate_dummy_block(9324);
        free_dummy_block();
        allocate_dummy_block(9085);
        free_dummy_block();
        _printf(".");
        allocate_dummy_block(39726);
        free_dummy_block();
        allocate_dummy_block(34627);
        free_dummy_block();
        allocate_dummy_block(47041);
        allocate_dummy_block(18106);
        free_dummy_block();
        allocate_dummy_block(57891);
        allocate_dummy_block(47589);
        allocate_dummy_block(24744);
        allocate_dummy_block(54181);
        allocate_dummy_block(69695);
        free_dummy_block();
        allocate_dummy_block(23159);
        free_dummy_block();
        _printf(".");
        allocate_dummy_block(36191);
        free_dummy_block();
        allocate_dummy_block(14695);
        free_dummy_block();
        allocate_dummy_block(10709);
        allocate_dummy_block(52911);
        free_dummy_block();
        allocate_dummy_block(38169);
        allocate_dummy_block(51211);
        free_dummy_block();
        allocate_dummy_block(56271);
        free_dummy_block();
        allocate_dummy_block(5852);
        allocate_dummy_block(64613);
        allocate_dummy_block(25953);
        _printf(".");
        allocate_dummy_block(3462);
        free_dummy_block();
        allocate_dummy_block(12668);
        allocate_dummy_block(50838);
        allocate_dummy_block(3888);
        free_dummy_block();
        allocate_dummy_block(20956);
        allocate_dummy_block(34583);
        allocate_dummy_block(13365);
        allocate_dummy_block(37252);
        free_dummy_block();
        allocate_dummy_block(23603);
        free_dummy_block();
        allocate_dummy_block(31510);
        _printf(".");
        allocate_dummy_block(7629);
        allocate_dummy_block(46032);
        free_dummy_block();
        allocate_dummy_block(6266);
        allocate_dummy_block(31463);
        allocate_dummy_block(17271);
        free_dummy_block();
        allocate_dummy_block(67031);
        free_dummy_block();
        allocate_dummy_block(47562);
        free_dummy_block();
        allocate_dummy_block(24622);
        allocate_dummy_block(44097);
        allocate_dummy_block(17173);
        free_dummy_block();
        _printf(".");
        allocate_dummy_block(6047);
        allocate_dummy_block(42198);
        free_dummy_block();
        allocate_dummy_block(60994);
        allocate_dummy_block(23242);
        allocate_dummy_block(15868);
        allocate_dummy_block(61315);
        allocate_dummy_block(56594);
        allocate_dummy_block(1844);
        free_dummy_block();
        allocate_dummy_block(63912);
        free_dummy_block();
        allocate_dummy_block(51708);
        free_dummy_block();
        _printf(".");
        allocate_dummy_block(16476);
        allocate_dummy_block(54679);
        allocate_dummy_block(40373);
        allocate_dummy_block(64791);
        free_dummy_block();
        allocate_dummy_block(69415);
        free_dummy_block();
        allocate_dummy_block(7057);
        allocate_dummy_block(61891);
        free_dummy_block();
        allocate_dummy_block(35963);
        free_dummy_block();
        allocate_dummy_block(34982);
        allocate_dummy_block(57250);
        _printf(".");
        allocate_dummy_block(13686);
        free_dummy_block();
        allocate_dummy_block(443);
        free_dummy_block();
        allocate_dummy_block(61564);
        allocate_dummy_block(49817);
        free_dummy_block();
        allocate_dummy_block(43223);
        free_dummy_block();
        allocate_dummy_block(29908);
        allocate_dummy_block(25401);
        allocate_dummy_block(27598);
        free_dummy_block();
        allocate_dummy_block(67139);
        allocate_dummy_block(1217);
        _printf(".");
        allocate_dummy_block(8509);
        allocate_dummy_block(8493);
        free_dummy_block();
        allocate_dummy_block(29680);
        free_dummy_block();
        allocate_dummy_block(66064);
        allocate_dummy_block(49141);
        free_dummy_block();
        allocate_dummy_block(9762);
        allocate_dummy_block(53706);
        free_dummy_block();
        allocate_dummy_block(43400);
        allocate_dummy_block(52494);
        allocate_dummy_block(49456);
        free_dummy_block();
        _printf(".");
        allocate_dummy_block(28986);
        free_dummy_block();
        allocate_dummy_block(7658);
        allocate_dummy_block(63519);
        free_dummy_block();
        allocate_dummy_block(50000);
        allocate_dummy_block(19462);
        free_dummy_block();
        allocate_dummy_block(21363);
        allocate_dummy_block(52638);
        allocate_dummy_block(23953);
        allocate_dummy_block(737);
        allocate_dummy_block(59796);
        free_dummy_block();
        _printf(".");
        allocate_dummy_block(47873);
        allocate_dummy_block(62679);
        free_dummy_block();
        allocate_dummy_block(64813);
        free_dummy_block();
        allocate_dummy_block(43155);
        allocate_dummy_block(33502);
        free_dummy_block();
        allocate_dummy_block(7542);
        allocate_dummy_block(38342);
        allocate_dummy_block(57685);
        allocate_dummy_block(31085);
        allocate_dummy_block(40941);
        _printf(".");
        allocate_dummy_block(7169);
        free_dummy_block();
        allocate_dummy_block(17585);
        allocate_dummy_block(10004);
        free_dummy_block();
        allocate_dummy_block(68086);
        free_dummy_block();
        allocate_dummy_block(62341);
        allocate_dummy_block(49040);
        free_dummy_block();
        allocate_dummy_block(18844);
        free_dummy_block();
        allocate_dummy_block(18668);
        free_dummy_block();
        allocate_dummy_block(4527);
        allocate_dummy_block(63873);
        _printf(".");
        allocate_dummy_block(30009);
        free_dummy_block();
        allocate_dummy_block(65669);
        allocate_dummy_block(48814);
        allocate_dummy_block(8192);
        allocate_dummy_block(44603);
        allocate_dummy_block(8415);
        allocate_dummy_block(34657);
        free_dummy_block();
        allocate_dummy_block(44710);
        allocate_dummy_block(47316);
}

/**
 * The memory viewer tool entry function.
 */
void memview_main(void)
{
        memset(mv_allocated_blocks, 0, sizeof(mv_allocated_blocks));

        int keyboard = _open("/dev/keyboard", 0, 0);
        int stdin = _open("/dev/stdin", 0, 0);

        // Get initial heap size and compute initial block size
        mv_total_mem = (uint32)kernel_heap->end + sizeof (mm_header) - (uint32)kernel_heap->start;
        mv_bytes_per_block = mv_total_mem / (25 * 80);

        _printf("Welcome to #{YEL}memview##, the #{BLU}POTATOES## #{GRE}visual memory viewer## tool!\n");
        _printf("Kernel heap from 0x%x to 0x%x\n", (uint32)kernel_heap->start, (uint32)kernel_heap->end + sizeof (mm_header));
        _printf("%dkb heap size\nNote: the kernel heap size changes as the heap expands and contracts\n", mv_total_mem / 1024);
        _printf("Initial resolution: 1 block = %dkb\n", mv_bytes_per_block / 1024);
        _printf("\n#{YEL}Usage:##\nu - update view\na - allocate block\nf - free block\ni - zoom in\no - zoom out\nb - do memory benchmark\ns - show statistics screen\nESCAPE - exit memview\n\n");
        _printf("#{YEL}Block types:##\n#{GRE}### - completely free region\n#{PIN}### - only partly occupied by first allocation\n");
        _printf("#{RED}### - completely filled region\n#{DAR}### - region outside kernel heap\n\n[Press any key to start]\n");
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
                        if (mv_bytes_per_block > 512) {
                                mv_bytes_per_block -= 512;
                                update_view();
                        }
                        break;

                case 'o': // zoom out
                        mv_bytes_per_block += 512;
                        update_view();
                        break;

                case 'u': // update view
                        update_view();
                        break;

                case 's': // stats screen
                        mv_switch_to_textmode();
                        mv_show_stats();
                        mv_switch_to_graphicsmode();
                        update_view();
                        break;

                case 'b': // memory manager benchmark
                        mv_switch_to_textmode();
                        mv_do_benchmark();
                        mv_switch_to_graphicsmode();
                        update_view();
                        break;
                }

                halt();
        }

        _close(mv_framebuf);
        free_all_dummy_blocks();

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
