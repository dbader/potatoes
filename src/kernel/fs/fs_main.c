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
#include "../include/debug.h"

#include "fs_const.h"
#include "fs_types.h"
#include "fs_file_table.h"
#include "fs_inode_table.h"
#include "fs_io_functions.h"
#include "fs_super.h"
#include "fs_bmap.h"
#include "fs_main.h"

extern void panic(char *msg);

//tests
extern void run_FS_tests();

/**
 * Initializes the file system.
 */
void fs_init()
{
        if(!load_fs()){
                dprintf("FS loading failed. trying to create a new one.\n");
                if (!create_fs()){
                        panic("file system cannot be initialized!\n");
                }
        }
        
        
}

bool fs_shutdown()
{
        write_root(); //TODO: intersection with "close all left inodes?"
        
        //close all open files
        for(int i = 0; i < NUM_FILES; i++){
                fs_close(gft[i].f_desc);
        }
        

        
        write_bmap();
        write_super_block();

        
        return TRUE; //TODO: void?
}

bool load_fs()
{
        dprintf("loading FS from HD\n");
        load_bmap();
        init_inode_table();
        load_root();
        load_super_block();

        return super_block.s_used;
}


bool create_fs()
{
        dprintf("starting to create new FS\n");
        dump_consts();
        
        reset_bmap();
        //test_bmap();
        init_inode_table();

        create_root();
        
        init_super_block();
        dump_super();
        
        //run tests
        run_FS_tests();
        return TRUE; //TODO: void?
}

void dump_consts()
{
        dprintf("NUM_BLOCKS_ON_HD = %d\nNUM_FILES = %d\nNUM_PROC_FILES = %d\nNUM_INODES = %d\n\n"
                "SUPER_SIZE = %d\nDISK_INODE_SIZE = %d\nMEM_INODE_SIZE = %d\n\n"
                "INODES_PER_BLOCK = %d\nDIR_ENTRIES_PER_BLOCK = %d\nADDRS_PER_BLOCK = %d\n"
                "BYTES_DIRECT = %d\nBYTES_SINGLE_INDIRECT = %d\nBYTES_DOUBLE_INDIRECT = %d\n\n"
                "NUM_BMAP_BLOCKS = %d\nROOT_INODE_BLOCK = %d\nFIRST_DATA_BLOCK = %d\n\n\n",
                NUM_BLOCKS_ON_HD, NUM_FILES, NUM_PROC_FILES, NUM_INODES,
                SUPER_SIZE, DISK_INODE_SIZE, MEM_INODE_SIZE,
                INODES_PER_BLOCK, DIR_ENTRIES_PER_BLOCK, ADDRS_PER_BLOCK,
                BYTES_DIRECT, BYTES_SINGLE_INDIRECT, BYTES_DOUBLE_INDIRECT,
                NUM_BMAP_BLOCKS, ROOT_INODE_BLOCK, FIRST_DATA_BLOCK);
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

