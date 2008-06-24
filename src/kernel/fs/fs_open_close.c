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
 * Basic definitions concerning syscalls "open" and "close"
 * 
 * @author Vincenz Doelle
 * @author $LastChangedBy$
 * @version $Rev$
 */

#include "../include/const.h"
#include "../include/types.h"
#include "../include/stdlib.h"
#include "../include/debug.h"

#include "fs_const.h"
#include "fs_types.h"
#include "fs_file_table.h"
#include "fs_inode_table.h"
#include "fs_buf.h"
#include "fs_block_dev.h"
#include "fs_io_functions.h"
#include "fs_dir.h"


/**
 * Opens a file by searching the inode's block number, 
 * loading the inode from HD, inserting the inode to the inode table and
 * and registering the file in the global/process file table.
 * 
 * @param path The absolute file path
 * @return The process file descriptor
 * 
 */
file_nr fs_open(char *abs_path)
{
        file_nr fd = name2desc(abs_path); //get the file descriptor from file table if already existent
        if (fd != NOT_FOUND){
                inc_count(fd);
                return fd; //file already exists
        }
        
        block_nr block = search_file(abs_path);
        if (block == NOT_FOUND){
                dprintf("block not found!\n");
                return NOT_EXISTENT;
        }
        
        m_inode *inode = alloc_inode();
        if (inode == (m_inode*) 0){
                dprintf("inode not allocateable!\n");
                return NOT_POSSIBLE;
        }
        
        read_minode(inode, block); //read content from HD
        
        fd = insert_file(inode, abs_path, inode->i_mode);
        if (fd == NOT_FOUND){
                dprintf("new file could not be inserted!\n");
                return NOT_POSSIBLE;
        }
        
        return fd;
}

/**
 * Closes a file by writing the inode to HD and freeing the file descriptor entry.
 * 
 * @param fd The process file descriptor.
 */
bool fs_close(file_nr fd)
{
        file *f = get_file(fd);
        if (f == NULL){
                return FALSE; //file does not exist
        }
        
        m_inode *inode = f->f_inode;
        
        write_inode(inode);
        
        free_file(fd);
        
        return TRUE;
}
