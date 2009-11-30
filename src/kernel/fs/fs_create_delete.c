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
 * Functions concerning syscalls "create" and "delete"
 *
 * @author Vincenz Doelle
 * @author $LastChangedBy$
 * @version $Rev$
 */

#include "../include/const.h"
#include "../include/types.h"
#include "../include/string.h"
#include "../include/stdlib.h"
#include "../include/debug.h"

#include "fs_const.h"
#include "fs_types.h"

#include "fs_block_dev.h"
#include "fs_buf.h"
#include "fs_bmap.h"
#include "fs_inode_table.h"
#include "fs_file_table.h"
#include "fs_dir.h"
#include "fs_io_functions.h"

/**
 * Creates a file from absolute path by inserting the name into the containing directory,
 * creating a new inode and writing it to HD.
 *
 * @param abs_path   absolute file path
 * @param data_type  DATA_FILE | DIRECTORY (@see fs_const.h)
 * @return           result status of the create operation
 */
bool fs_create(char *abs_path, int data_type)
{
        return fs_create_delete(abs_path, CREATE, data_type);
}


/**
 * Deletes a file by removing it from the containing directory.
 *
 * @param abs_path absolute file path
 * @return         result status of the delete operation
 */
bool fs_delete(char *abs_path)
{
        return fs_create_delete(abs_path, DELETE, NULL);
}

/**
 * Handles the creation and deletion process.
 *
 * @param abs_path  absolute file path
 * @param mode      CREATE | DELETE (@see fs_const.h)
 * @param data_type DATA_FILE | DIRECTORY (@see fs_const.h)
 * @return          result status of the create/delete operation
 */
bool fs_create_delete(char *abs_path, int mode, int data_type)
{
        if (mode == CREATE) {
                fs_dprintf("[fs_create_delete] CREATING file '%s'...\n", abs_path);
        } else {
                fs_dprintf("[fs_create_delete] DELETING file '%s'...\n", abs_path);
        }

        if (mode == CREATE && search_file(abs_path) != NOT_FOUND) {
                printf("%{ERROR}: file '%s' already exists!\n", RED, abs_path);
                return FALSE;
        }

        if (mode == DELETE && search_file(abs_path) == NOT_FOUND) {
                printf("%{ERROR}: file '%s' does not exist!\n", RED, abs_path);
                return FALSE;
        }

        char *path = get_path(abs_path);
        if (path == NULL) {
                return FALSE;
        }

        block_nr dir_inode_block;

        if (strcmp(path, "/") == 0) {
                dir_inode_block = ROOT_INODE_BLOCK;
        } else {
                dir_inode_block = search_file(path); //find dir
        }

        if (dir_inode_block == NOT_FOUND) {
                return FALSE;
        }

        char *file_name = get_filename(abs_path);
        if (file_name == NULL) {
                return FALSE;
        }

        block_nr file_block;

        if (mode == CREATE) {
                file_block = insert_file_into_dir(dir_inode_block, file_name);
        } else if (mode == DELETE) {
                fs_truncate(abs_path, 0);
                file_block = delete_file_from_dir(dir_inode_block, file_name);
        }

        if (file_block == NOT_POSSIBLE) {
                return FALSE;
        }

        if (mode == CREATE) {
                //create new inode and write it to HD
                fs_dprintf("[fs_c_d] create new inode (type = %d) and write it to block %d\n", data_type, file_block);
                m_inode *inode = new_minode(file_block, data_type, FALSE);

                if (inode == NULL) {
                        return FALSE;
                }

                write_inode(inode);

                free(inode); //free memory, load from HD if needed
        }

        free(file_name);
        free(path);

        return TRUE;
}

/**
 * Mark all the children of @a block after and including @a start as unused.
 *
 * If start == 0 also mark the block itself as unused and write NULL to @a block.
 */
static void shrink_file(block_nr *block, uint32 start)
{
        fs_dprintf("[shrink_file] shrink_file(&%x, %x).\n", *block, start);

        if(start >= ADDRS_PER_BLOCK)
        {
                fs_dprintf("[shrink_file] superflously called.\n");
                return; // nothing to do.
        }

        block_nr addr_cache2[ADDRS_PER_BLOCK];

        rd_block(addr_cache2, *block, sizeof(addr_cache2));

        int i;
        for(i = start; i < ADDRS_PER_BLOCK; ++i)
        {
                if(addr_cache2[i] != NULL)
                {
                        mark_block(addr_cache2[i], FALSE); // set block as unused
                        addr_cache2[i] = NULL;
                }
        }

        if(start != 0)
        {
                // the block is still used; write the new block.
                wrt_block(*block, addr_cache2, sizeof(addr_cache2));
        }
        else
        {
                // the block itself is obsolete; mark it as unused and delete it from its parent.
                mark_block(*block, FALSE); // set block as unused
                *block = NULL;
                // no need to write the block: nothing should reference it anymore.
        }
}

/**
 * Set the size of file @a abs_path to be exactly @a size, deleting or allocating blocks as necessary.
 */
bool fs_truncate(char *abs_path, uint32 size)
{
        fs_dprintf("[fs_truncate] fs_truncate(%s, %x)", abs_path, size);

        int fd = fs_open(abs_path);
        if(fd < 0)
            return FALSE;

        m_inode *inode = get_file(fd)->f_inode;

        if(size < inode->i_size)
        {
                int start = (size + BLOCK_SIZE - 1) / BLOCK_SIZE; // first block to be freed
                int i, j, blk;

                // delete all obsolete direct pointers.
                for(i = start; i < NUM_DIRECT_POINTER; ++i)
                {
                        blk = inode->i_direct_pointer[i];
                        if(blk != NULL)
                        {
                            mark_block(blk, FALSE); // set block as unused
                            inode->i_direct_pointer[i] = NULL;
                        }
                }

                // delete all indirect pointers and the indirect block itself, if necessary.
                if(inode->i_single_indirect_pointer != NULL && start < NUM_DIRECT_POINTER + ADDRS_PER_BLOCK)
                        shrink_file(&inode->i_single_indirect_pointer, MAX(0, start - NUM_DIRECT_POINTER));

                // delete all doubly indirect pointers.
                if(inode->i_double_indirect_pointer != NULL)
                {
                        rd_block(addr_cache, inode->i_double_indirect_pointer, sizeof(addr_cache));

                        for(j = MAX(0, (start - NUM_DIRECT_POINTER - ADDRS_PER_BLOCK) / ADDRS_PER_BLOCK); j < ADDRS_PER_BLOCK; ++j)
                        {
                                if(addr_cache[j] == NULL)
                                        break;

                                shrink_file(&addr_cache[j], MAX(0, start - NUM_DIRECT_POINTER - (j + 1) * ADDRS_PER_BLOCK));
                        }

                        if(start < NUM_DIRECT_POINTER + ADDRS_PER_BLOCK)
                        {
                                // the block is obsolete; mark it as unused.
                                mark_block(inode->i_double_indirect_pointer, FALSE);
                                inode->i_double_indirect_pointer = NULL;
                        }
                        else
                        {
                                // we still need the block; write it back to disk.
                                wrt_block(inode->i_double_indirect_pointer, addr_cache, sizeof(addr_cache)); // write changes
                        }
                }

        }
        else if(size > inode->i_size)
                get_data_block(inode, size, TRUE);

        inode->i_size = size;
        write_inode(inode);

        fs_close(fd);

        return TRUE;
}
