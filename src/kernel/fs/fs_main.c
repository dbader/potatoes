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
 * This is the central program dealing as an interface between FS and PM.
 * 
 * 
 * @author Vincenz Doelle
 * @author $LastChangedBy$
 * @version $Rev$
 */

#include "../include/const.h"
#include "../include/types.h"
#include "../include/debug.h"
#include "../include/stdlib.h"

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
        dprint_separator();
        dprintf("%{FS:} init\n", GREEN);
        
        if(!load_fs()){
                printf("%{FS:} %{FS loading failed.}\n", GREEN, RED);
                if (!create_fs()){
                        panic("FS cannot be initialized!\n");
                }
        }
                
}

/**
 * Shuts the file system down and writes all important information to HD.
 */
void fs_shutdown()
{
        printf("%{FS:} shutting down FS..\n", GREEN);
        write_root(); //TODO: intersection with "close all left inodes?"
        
        //close all open files
        for(int i = 0; i < NUM_FILES; i++){
                fs_close(gft[i].f_desc); //TODO: problem when count > 1 (e.g. not properly closed by PM)
        }
        
        write_bmap();
        write_super_block();
        free(bmap);
}

/**
 * Loads the file system from HD.
 */
bool load_fs()
{
        printf("%{FS:} loading FS from HD..\n", GREEN);
        load_bmap();
        init_inode_table();
        init_file_table();
        load_root();
        load_super_block();

        return (super_block.s_magic_number == MAGIC_NUMBER);
}

/**
 * Creates a new file system.
 */
bool create_fs()
{
        printf("%{FS:} creating new FS...\n", GREEN);
        //dump_consts();

        init_bmap();
        init_inode_table();
        init_file_table();
        create_root();
        init_super_block();
        dump_super();
        
        return TRUE; 
}

/**
 * interface functions to handle the system calls.
 */

size_t do_read(file_nr fd, void *buf, size_t count, uint32 pos)
{
        return fs_read(buf, get_file(fd)->f_inode, count, pos, FALSE);
}

size_t do_write(file_nr fd, void *buf, size_t count, uint32 pos)
{
        return fs_write(get_file(fd)->f_inode, buf, count, pos, TRUE);
}

bool do_create(char *abs_path, uint8 mode)
{
        return (fs_create(abs_path, mode));
}

bool do_mkdir(char *abs_path)
{
        return (fs_create(abs_path, DIRECTORY));
}

bool do_mkfile(char *abs_path)
{
        return (do_create(abs_path, DATA_FILE));
}

int do_remove(char *abs_path)
{
        return (fs_delete(abs_path)) ? 0 : -1;
}

file_nr do_open(char *abs_path)
{
        return (fs_open(abs_path));
}

int do_close(file_nr fd)
{
        return (fs_close(fd)) ? 0 : EOF;
}

int do_close_pf(proc_file pft[NUM_PROC_FILES], file_nr pfd)
{
        proc_file *pf = get_proc_file(pft, pfd);
        if (do_close(pf->pf_f_desc) == 0){
                free_proc_file(pft, pfd);
                return 0;
        }
        
        return EOF;
}


/**
 * Prints out all important constants concerning the file system.
 * For debug purposes only.
 */ 
void dump_consts()
{
        printf("NUM_FILES = %d\nNUM_PROC_FILES = %d\nNUM_INODES = %d\n\n"
                "SUPER_SIZE = %d\nDISK_INODE_SIZE = %d\nMEM_INODE_SIZE = %d\n\n"
                "INODES_PER_BLOCK = %d\nDIR_ENTRIES_PER_BLOCK = %d\nADDRS_PER_BLOCK = %d\n"
                "BYTES_DIRECT = %d\nBYTES_SINGLE_INDIRECT = %d\nBYTES_DOUBLE_INDIRECT = %d\n\n"
                "ROOT_INODE_BLOCK = %d\n\n\n",
                NUM_FILES, NUM_PROC_FILES, NUM_INODES,
                SUPER_SIZE, DISK_INODE_SIZE, MEM_INODE_SIZE,
                INODES_PER_BLOCK, DIR_ENTRIES_PER_BLOCK, ADDRS_PER_BLOCK,
                BYTES_DIRECT, BYTES_SINGLE_INDIRECT, BYTES_DOUBLE_INDIRECT,
                ROOT_INODE_BLOCK);
}

