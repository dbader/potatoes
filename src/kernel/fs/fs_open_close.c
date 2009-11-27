/* $Id: fs_open_close.c 266 2009-10-14 08:15:36Z dtraytel $
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
 * Basic definitions concerning syscalls "open" and "close"
 *
 * @author Vincenz Doelle
 * @author $LastChangedBy: dtraytel $
 * @version $Rev: 266 $
 */

#include "../include/const.h"
#include "../include/types.h"
#include "../include/stdlib.h"
#include "../include/string.h"
#include "../include/debug.h"

#include "fs_const.h"
#include "fs_types.h"
#include "fs_io_functions.h"
#include "fs_file_table.h"
#include "fs_inode_table.h"
#include "fs_buf.h"
#include "fs_block_dev.h"
#include "fs_dir.h"


/**
 * Opens a file by searching the inode's block number,
 * loading the inode from HD, inserting the inode to the inode table and
 * and registering the file in the global/process file table.
 *
 * @param abs_path      absolute file path
 * @return              process file descriptor
 */
file_nr fs_open(char *abs_path)
{
        file_nr fd;
        m_inode *inode;

        fs_dprintf("[fs_o_c] trying to open %s\n", abs_path);

        if (strcmp(abs_path, "/") != 0) { //not the root directory
                fd = name2desc(abs_path); //get the file descriptor from file table if already existent
                if (fd != NOT_FOUND) { //file already exists
                        fs_dprintf("[fs_o_c] file %s is already opened!\n", abs_path);
                        return fd;
                }

                block_nr block = search_file(abs_path);
                if (block == NOT_FOUND) {
                        fs_dprintf("[fs_o_c] block not found!\n");
                        return NOT_EXISTENT;
                }

                inode = alloc_inode();
                if (inode == (m_inode*) NULL) {
                        fs_dprintf("[fs_o_c] inode not allocateable!\n");
                        return NOT_POSSIBLE;
                }

                read_minode(inode, block); //read content from HD
        } else {
                inode = root;
        }

        fd = insert_file(inode, abs_path, inode->i_mode);
        if (fd == NOT_FOUND) {
                fs_dprintf("[fs_o_c] new file could not be inserted!\n");
                return NOT_POSSIBLE;
        }
        return fd;
}

/**
 * Closes a file by writing the inode to HD and freeing the file descriptor entry.
 *
 * @param fd    process file descriptor.
 * @return      operation status
 */
bool fs_close(file_nr fd)
{
        file *f = get_file(fd);

        if (f == (file*) NULL || f->f_desc == NIL_FILE) {
                return FALSE; //file does not exist
        }
        
        fs_dprintf("CLOSING file %s\n", f->f_name);
        
        write_inode(f->f_inode);
        free_file(fd);
        
        dump_files();

        return TRUE;
}
