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
 * The file descriptor table functions.
 *
 * @author Vincenz Doelle
 * @author $LastChangedBy$
 * @version $Rev$
 */
#include "../include/const.h"
#include "../include/types.h"
#include "../include/string.h"

#include "const.h"
#include "types.h"
#include "filp.h"

/**
 * Initializes the global filp table with NULL elements
 */
void init_file_table()
{
        for(int i = 0; i < NUM_FILES; i++){
                gft[i].f_desc = NIL_FILE;
        }
}

/**
 * Initializes the process filp table with NULL elements
 */
void init_proc_file_table(proc_file pft[NUM_PROC_FILES])  //TODO: proc_file_table *pft ? --> typedef proc_file_table
{
        for(int i = 0; i < NUM_PROC_FILES; i++){
                pft[i].pf_desc = NIL_PROC_FILE;
        }
}

/**
 * Inserts a new file to the global filp table
 * 
 * @return desc - the assigned file descriptor
 */
sint16 insert_file(m_inode *inode, char *name, uint8 mode) //length: NUM_FILES
{
        sint16 fd = alloc_file();
        
        if (fd == NOT_FOUND){
                return NOT_POSSIBLE;
        }
        if (contains_file(fd) == TRUE){
                inc_count(fd);
        } else {
                gft[fd].f_desc  = fd;
                gft[fd].f_inode = inode;
                gft[fd].f_name  = name;
                gft[fd].f_mode  = mode;
                gft[fd].f_count = 1;
        }
        
        return fd;
}


/**
 * Inserts a new file to a given process filp table
 * 
 * @return fd - the assigned file descriptor
 */
sint16 insert_proc_file(proc_file pft[NUM_PROC_FILES], uint16 glo_fd) //length: NUM_PROC_FILES
{
        sint16 fd = alloc_proc_file(pft);
        
        if (fd == NOT_FOUND){
                return NOT_POSSIBLE;
        }
        
        pft[fd].pf_desc   = fd;
        pft[fd].pf_f_desc = glo_fd;
        pft[fd].pf_pos    = 0;
        
        return fd;
}

/**
 * Finds an unused descriptor in the global filp table
 * 
 * @return desc - the found descriptor
 */
sint16 alloc_file()
{
        for(int i = 0; i < NUM_FILES; i++){
                if (gft[i].f_desc == NIL_FILE){
                        return conv_desc(i);
                }
        }
        return NOT_FOUND;
}

/**
 * Finds an unused descriptor in the process filp table
 * 
 * @return desc - the found descriptor
 */
sint16 alloc_proc_file(proc_file pft[NUM_PROC_FILES])
{
        for(int i = 0; i < NUM_PROC_FILES; i++){
                if (pft[i].pf_desc == NIL_PROC_FILE){
                        return conv_desc(i);
                }
        }
        return NOT_FOUND;  
}

/**
 * Finds a file in the global filp table
 * 
 * @return *file - the found file
 */

file* get_file(uint16 fd)
{
        for (int i = 0; i < NUM_FILES; i++){
                if ( gft[i].f_desc == fd )
                        return &gft[i];
        } 
               
        return (file *) 0;  
}

/**
 * Increments the reference counter of a file
 */
void inc_count(uint16 fd)
{
        file *f = get_file(fd);
        f->f_count++;
}

/**
 * Finds the descriptor of a file with the file name
 * 
 * @return fd - the found file descriptor
 */
sint16 name2desc(char *name) //in global filp table
{ 
        int i;
        for (i = 0; i < NUM_FILES; i++){
               /*TODO: enable when strcmp is implemented 
                * if (strcmp(name, gft[i].f_name) == 0) 
                *        return conv_desc(i);
                */
        } 
        
        return NOT_FOUND; 
}

/**
 * Finds the descriptor of a file with the inode (number)
 * 
 * @return fd - the found file descriptor
 */
sint16 inode2desc(m_inode *inode)
{
        for (int i = 0; i < NUM_FILES; i++){
                if ( inode->i_num == (gft[i].f_inode)->i_num )
                        return conv_desc(i);
        } 
               
        return NOT_FOUND; 
}

/**
 * Looks whether the global filp table contains a special file descriptor
 * 
 * @return TRUE | FALSE
 */
bool contains_file(uint16 fd)
{
        return ( get_file(fd) != (file *) 0);
}

/**
 * Converts a filp table entry number to the really used descriptor
 * 
 * @return fd = fd + FS_OFFSET
 */
uint16 conv_desc(uint16 fd)
{
        return fd + FD_OFFSET;
}
