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
 * FS main.
 * 
 * @author Vincenz Doelle
 * @author $LastChangedBy$
 * @version $Rev$
 */

#include "../include/const.h"
#include "../include/types.h"

#include "fs_const.h"
#include "fs_types.h"
#include "fs_file_table.h"
#include "fs_inode_table.h"
#include "fs_io_functions.h"
#include "fs_super.h"
#include "fs_bmap.h"
#include "fs_main.h"

extern void panic(char *msg);

/**
 * Initializes the file system.
 */
void fs_init()
{
        /*if(!load_fs()){
                if (!create_fs()){
                        panic("file system cannot be initialized!");
                }
        }*/
}

bool fs_shutdown()
{
        write_root(); //TODO: intersection with "close all left inodes?"
        
        //close all open files
        for(int i = 0; i < NUM_FILES; i++){
                fs_close(gft[i].f_desc);
        }
        
        //close all left inodes
        for (int i = 0; i < NUM_INODES; i++){
                write_inode(&inode_table[i]);
        }
        
        write_bmap();
        write_super_block();

        
        return TRUE; //TODO: void?
}

bool load_fs()
{
        load_bmap();
        init_inode_table();
        load_root();
        load_super_block();
        
        return super_block.s_used;
}

bool create_fs()
{
        reset_bmap();
        init_inode_table();
        create_root();
        init_super_block();
        
        return TRUE; //TODO: void?
}

bool do_read(void *buf, file_nr file, size_t num_bytes)
{
        //TODO: do_read
}

bool do_write(file_nr file, void *buf, size_t num_bytes)
{
        //TODO: do_write
}

//TODO: implement other do_x functions for sys-calls

