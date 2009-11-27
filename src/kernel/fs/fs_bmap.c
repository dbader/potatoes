/* $Id: fs_bmap.c 266 2009-10-14 08:15:36Z dtraytel $
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
 * Functions to manage the block bitmap (bmap)
 *
 * @author Vincenz Doelle
 * @author $LastChangedBy: dtraytel $
 * @version $Rev: 266 $
 */

#include "../include/const.h"
#include "../include/types.h"
#include "../include/debug.h"
#include "../include/assert.h"
#include "../include/string.h"
#include "../include/stdlib.h"

#include "fs_const.h"
#include "fs_types.h"

#include "fs_super.h"
#include "fs_bmap.h"
#include "fs_block_dev.h"

extern uint32 get_hdsize();

/**
 * Allocate the block bitmap in memory.
 *
 * @return size of the block bitmap
 */
size_t malloc_bmap()
{
        size_t size =  get_hdsize() / 8; //manage 8 blocks with one byte (see 'bitmap')
        num_bmap_blocks = size / BLOCK_SIZE + 1; // '+ 1' to round up
        first_data_block = ROOT_INODE_BLOCK + num_bmap_blocks + 1;

        bmap = mallocn(size, "block bitmap");

        //SHORTCUT_CTRL('b',dump_bmap);
        if (bmap == NULL) {
                return 0;
        } else {
                fs_dprintf("[fs_bmap] malloced bmap.\n");
                bzero(bmap, size);

                return size;
        }
}

/**
 * Resets the block bitmap.
 */
void init_bmap()
{
        size_t size = malloc_bmap();
        ASSERT(size != 0);

        fs_dprintf("[fs_bmap] initializing bmap (size = %d)\n", size);

        //Set boot block, super block, ... (until first "real" data block) as used.
        for (block_nr i = 0; i < first_data_block; i++) {
                mark_block(i, TRUE);
        }
}

/**
 * Loads the block bitmap from HD
 */
void load_bmap()
{
        size_t size = malloc_bmap();
        ASSERT(size != 0);

        fs_dprintf("[fs_bmap] loading bmap (size = %d) from HD\n", size);
        int j = 0;
        for (int i = FIRST_BMAP_BLOCK; i < FIRST_BMAP_BLOCK + num_bmap_blocks; i++, j++) {
                rd_block(&bmap[j*BLOCK_SIZE], i, BLOCK_SIZE);
        }
}

/**
 * Writes the block bitmap to HD.
 */
void write_bmap()
{
        fs_dprintf("[fs_bmap] writing bmap to HD\n");
        int j = 0;
        for (int i = FIRST_BMAP_BLOCK; i < FIRST_BMAP_BLOCK + num_bmap_blocks; i++, j++) {
                wrt_block(i, &bmap[j*BLOCK_SIZE], BLOCK_SIZE);
        }
}

/**
 * Find a new unused block.
 * Search linear from the "start" block number.
 *
 * @param start  search offset (start to search linear from block number "start")
 *
 * @return block number of the unused block
 */
block_nr get_free_block(block_nr start)
{
        block_nr blk = start;

        while (blk < get_hdsize()) {
                if (is_allocated_block(blk) == FALSE) {
                        return blk;
                }
                blk++;

        }
        return NOT_FOUND;
}

/**
 * Function to mark a block number as free (flag = 0) or used (flag = 1).
 *
 * @param blk_nr block number which should be marked
 * @param flag   value with whom the block should be marked (1 | 0)
 */
void mark_block(block_nr blk_nr, bool flag)
{
        uint8 byte = bmap[blk_nr / 8];
        uint8 bit  = blk_nr % 8;

        byte = (byte >> (8 - bit)) | (byte << bit); //rotate left

        if (flag == TRUE) {
                byte = byte | 0x80;  //set first bit to 1 with (byte OR 10000000)
        } else {
                byte = byte & 0x7F;  //set first bit to 0 with (byte AND 01111111)
        }

        byte = (byte >> bit) | (byte << (8 - bit)); //rotate right (back)

        bmap[blk_nr / 8] = byte;

        //to ensure data consistency
        fs_dprintf("[fs_bmap] updating block nr %d at byte %d\n", FIRST_BMAP_BLOCK + (blk_nr / 8 / BLOCK_SIZE), (blk_nr / 8 / BLOCK_SIZE) * BLOCK_SIZE);
        wrt_block(FIRST_BMAP_BLOCK + (blk_nr / 8 / BLOCK_SIZE), &bmap[(blk_nr / 8 / BLOCK_SIZE) * BLOCK_SIZE], BLOCK_SIZE);
}

/**
 * Checks whether a block was already allocated before.
 *
 * @param blk_nr block number to check
 * @return       allocation status
 */
bool is_allocated_block(block_nr blk_nr)
{
        uint8 byte = bmap[blk_nr / 8];
        uint8 bit  = blk_nr % 8;

        byte = byte << bit;
        byte = byte & 0x80;

        return (byte != 0);
}

/**
 * Allocates a new block.
 *
 * @param start  search offset (start to search linear from block number "start")
 */
block_nr alloc_block(block_nr start)
{
        block_nr blk_nr;
        blk_nr = get_free_block(start);
        mark_block(blk_nr, TRUE);
        return blk_nr;
}

/**
 * Dumps out the block bitmap.
 */
void dump_bmap()
{
        uint8 byte = 0;
        uint8 bit  = 0;

        if (bmap == NULL) {
                fs_dprintf("[fs_bmap] BMAP is NULL\n");
                return;
        }

        fs_dprintf("[fs_bmap] BMAP (used blocks): \n");

        for (block_nr blk_nr = BOOT_BLOCK; blk_nr < get_hdsize(); blk_nr++) {
                if (is_allocated_block(blk_nr)) {
                        fs_dprintf("[%d] ", blk_nr);
                }
        }
        fs_dprintf("\n");
}
