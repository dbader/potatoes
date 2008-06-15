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
 * @param path The file's absolute path
 * @return The result status of the create operation
 */
bool fs_create(char *abs_path, int data_type)
{
        
        char *path = get_path(abs_path);
        block_nr dir_inode_block;
        
        if (strcmp(path, "/") == 0){
                dir_inode_block = ROOT_INODE_BLOCK;
        } else {
                dir_inode_block = search_file(path); //find dir
        }
        
        dprintf("[fs_c_d] found dir_inode_block: %d\n", dir_inode_block);
        
        if (dir_inode_block == NOT_FOUND){
                return FALSE;
        }
        
        block_nr file_block = insert_file_into_dir(dir_inode_block, get_filename(abs_path));
        
        if (file_block == NOT_POSSIBLE){
                return FALSE;
        }
        
        //create new inode and write it to HD
        dprintf("create new inode (type = %d) and write it to block %d\n", data_type, file_block);
        m_inode *inode = new_minode(file_block, data_type, FALSE); //TODO: store in inode table?
        write_inode(inode);
        //dump_inode(inode);
        free(inode); //free memory, load from HD if needed
        
        return TRUE;
}

/**
 * Deletes a file by deregistering it from the containing directory.
 * 
 * @param path  The absolute file path
 * @return      The operation's result status
 */
bool fs_delete(char *path)
{
        block_nr dir_blk = search_file(get_path(path));
        
}
