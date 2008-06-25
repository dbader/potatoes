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
#include "../include/stdlib.h"
#include "../include/debug.h"

#include "fs_const.h"
#include "fs_types.h"
#include "fs_file_table.h"

/**
 * Initialize the global filp table with NULL elements
 */
void init_file_table()
{
        for(int i = 0; i < NUM_FILES; i++){
                gft[i].f_desc = NIL_FILE;               //desc = NIL_FILE = -1 => desc not assigned => file not used
        }
}

void dump_file(file *f)
{
        dprintf("file %s:\nf_desc = %d\t f_inode (nr)= %d\t f_count = %d\t f_mode = %d\n",
                 f->f_name, f->f_desc, (f->f_inode)->i_num, f->f_count, f->f_mode);
}

void dump_files(){
        for (int i = 0; i < NUM_FILES; i++){
                dump_file(&gft[i]);
        }
}

/**
 * Initialize the process filp table with NULL elements
 */
void init_proc_file_table(proc_file pft[NUM_PROC_FILES])
{
        for(int i = 0; i < NUM_PROC_FILES; i++){
                pft[i].pf_desc = NIL_PROC_FILE;         //desc = NIL_PROC_FILE = -1 => desc not assigned => file not used
        }
}

void dump_proc_file(proc_file *pf)
{
        dprintf("proc_file:\npf_desc = %d\t pf_f_desc %d\t pf_pos = %d\n",
                 pf->pf_desc, pf->pf_f_desc, pf->pf_pos);
}

void dump_proc_files(proc_file pft[NUM_PROC_FILES]){
        for (int i = 0; i < NUM_PROC_FILES; i++){
                dump_proc_file(&pft[i]);
        }
}

/**
 * Insert a new file to the global filp table
 * 
 * @return desc The assigned file descriptor
 */
file_nr insert_file(m_inode *inode, char *name, uint8 mode)
{
        file_nr old = name2desc(name);          //exists a file with the same path already?
        if (old != NOT_FOUND){
                inc_count(old);                 //increment reference counter
                dprintf("file already exists. returned old FD.\n");
                return old;
        }
                
        file *fd = alloc_file();
        
        if (fd == (file*) NULL){
                dprintf("new file could not be allocated!\n");
                return NOT_POSSIBLE;
        }

        fd->f_inode = inode;
        fd->f_name  = name;
        fd->f_mode  = mode;
        
        return fd->f_desc;
}


/**
 * Insert a new file to a given process filp table
 * 
 * @return fd The assigned file descriptor
 */
file_nr insert_proc_file(proc_file pft[NUM_PROC_FILES], file_nr glo_fd) //length: NUM_PROC_FILES
{
        proc_file *fd = alloc_proc_file(pft);
        
        if (fd == (proc_file*) NULL){
                return NOT_POSSIBLE;
        }
        
        fd->pf_f_desc = glo_fd;
        fd->pf_pos    = 0;

        return fd->pf_desc;
}

/**
 * Allocate an unused file in the global file table.
 * 
 * @return Pointer to the allocated file
 */
file* alloc_file()
{
        for(int i = 0; i < NUM_FILES; i++){
                if (gft[i].f_desc == NIL_FILE){
                        gft[i].f_desc = conv_desc(i);
                        gft[i].f_count = 1;
                        return &gft[i];
                }
        }
        return (file*) NULL;
}

/**
 * Allocate an unused file in the process file table.
 * 
 * @return Pointer to the allocated file
 */
proc_file* alloc_proc_file(proc_file pft[NUM_PROC_FILES])
{
        for(int i = 0; i < NUM_PROC_FILES; i++){
                if (pft[i].pf_desc == NIL_PROC_FILE){
                        pft[i].pf_desc = conv_desc(i);
                        return &pft[i];
                }
        }
        return (proc_file*) NULL;
}

/**
 * Find a file in the global file table
 * 
 * @return Pointer to the found file
 */

file* get_file(file_nr fd)
{
        for (int i = 0; i < NUM_FILES; i++){
                if ( gft[i].f_desc == fd )
                        return &gft[i];
        } 
               
        return (file*) 0;  
}

/**
 * Find a file in the process file table
 * 
 * @return Pointer to the found file
 */

proc_file* get_proc_file(proc_file pft[NUM_PROC_FILES], file_nr fd)
{
        for (int i = 0; i < NUM_PROC_FILES; i++){
                if ( pft[i].pf_desc == fd )
                        return &pft[i];
        } 
               
        return (proc_file*) 0;  
}

void free_file(file_nr fd)
{
        file *f = get_file(fd);
        f->f_count--;
        if (f->f_count == 0){
                f->f_desc = NIL_FILE;
                free(f->f_name);
        }
}

void free_proc_file(proc_file pft[NUM_PROC_FILES], file_nr fd)
{
        proc_file *pf = get_proc_file(pft, fd);
        
        pf->pf_desc = NIL_PROC_FILE;
        pf->pf_pos = 0;
        
        free_file(pf->pf_desc);
        pf->pf_f_desc = NIL_FILE;
}

/**
 * Increment the reference counter of a file
 * 
 * @param fd The file descriptor.
 */
void inc_count(file_nr fd)
{
        file *f = get_file(fd);
        f->f_count++;
}

/**
 * Move file position to current file position + offset. 
 *
 * @param pft    A process file table
 * @param fd     A file descriptor
 * @param offset The offset
 */
void lseek(proc_file pft[NUM_PROC_FILES], file_nr fd, sint32 offset, uint32 origin)
{
        proc_file *pf = get_proc_file(pft, fd);
        pf->pf_pos = origin + offset;
}

/**
 * Find the descriptor of a file with the file name
 * 
 * @param name  The file name
 * @return fd   The found file descriptor
 */
file_nr name2desc(char *name) //in global filp table
{ 
        for (int i = 0; i < NUM_FILES; i++){
                if (strcmp(name, gft[i].f_name) == 0) 
                        dprintf("%s and %s are equal -> return %d\n", name, gft[i].f_name, gft[i].f_desc);
                        return gft[i].f_desc;
        } 
        
        return NOT_FOUND; 
}

/**
 * Find the descriptor of a file with the inode (number)
 * 
 * @param  inode  The inode     
 * @return fd     The found file descriptor
 */
file_nr inode2desc(m_inode *inode)
{
        for (int i = 0; i < NUM_FILES; i++){
                if ( inode->i_num == ((m_inode *)(gft[i].f_inode))->i_num ){
                        return gft[i].f_desc;
                }
        } 
               
        return NOT_FOUND; 
}

/**
 * Look whether the global file table contains a special file descriptor
 * 
 * @return TRUE | FALSE
 */
bool contains_file(file_nr fd)
{
        return ( get_file(fd) != (file *) 0);
}

/**
 * Convert a file table entry number to the real descriptor
 * 
 * @param  fd The file descriptor
 * @return fd = fd + FS_OFFSET
 */
file_nr conv_desc(file_nr fd)
{
        return fd + FD_OFFSET;
}
