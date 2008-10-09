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
#include "fs_inode_table.h"


/**
 * Initialize the global file table with NULL elements.
 */
void init_file_table()
{
        for (int i = 0; i < NUM_FILES; i++) {
                gft[i].f_desc = NIL_FILE;               //desc = NIL_FILE = -1 => desc not assigned => file not used
        }
}

/**
 * Initialize the process file table with NULL elements.
 * 
 * @param pft   process file table
 */
void init_proc_file_table(proc_file pft[NUM_PROC_FILES])
{
        fs_dprintf("[fs_file_table] initialize proc_file_table at 0x%p\n", pft);
        for (int i = 0; i < NUM_PROC_FILES; i++) {
                pft[i].pf_desc = NIL_PROC_FILE;         //desc = NIL_PROC_FILE = -1 => desc not assigned => file not used
        }
}

/**
 * Insert a new file to the global file table.
 *
 * @param inode corresponding inode
 * @param name  filename
 * @param mode  DATA_FILE | DIRECTORY
 * @return      assigned file descriptor
 */
file_nr insert_file(m_inode *inode, char *name, uint8 mode)
{
        name = strdup(name);

        file_nr old = name2desc(name);          //exists a file with the same path already?
        if (old != NOT_FOUND) {
                //inc_count(old);                 //increment reference counter
                fs_dprintf("[fs_file_table] file already exists. returned old FD.\n");
                free(name);
                return old;
        }

        file *fd = alloc_file();

        if (fd == (file*) NULL) {
                fs_dprintf("[fs_file_table] new file could not be allocated!\n");
                free(name);
                return NOT_POSSIBLE;
        }

        fd->f_inode = inode;
        fd->f_name  = name;
        fd->f_mode  = mode;

        return fd->f_desc;
}


/**
 * Insert a new file to a given process file table.
 *
 * @param pft      process file table
 * @param glo_fd   global (for all processes) file descriptor
 * @return         assigned file descriptor
 */
file_nr insert_proc_file(proc_file pft[NUM_PROC_FILES], file_nr glo_fd) //length: NUM_PROC_FILES
{
        proc_file *fd = alloc_proc_file(pft);

        if (fd == NULL) {
                fs_dprintf("allocation of new proc_file not possible!\n");
                return NOT_POSSIBLE;
        }

        fd->pf_f_desc = glo_fd;
        fd->pf_pos    = 0;

        file *f = get_file(glo_fd);

        if (f != NULL) {
                inc_count(glo_fd);
        }

        return fd->pf_desc;
}

/**
 * Allocate an unused file in the global file table.
 *
 * @return pointer to the allocated file
 */
file* alloc_file()
{
        for (int i = 0; i < NUM_FILES; i++) {
                if (gft[i].f_desc == NIL_FILE) {
                        gft[i].f_desc = i;
                        gft[i].f_count = 0;
                        return &gft[i];
                }
        }
        return (file *) NULL;
}

/**
 * Allocate an unused file in the process file table.
 *
 * @param pft   process file table
 * @return      pointer to the allocated file
 */
proc_file* alloc_proc_file(proc_file pft[NUM_PROC_FILES])
{
        for (int i = 0; i < NUM_PROC_FILES; i++) {
                if (pft[i].pf_desc == NIL_PROC_FILE) {
                        pft[i].pf_desc = i;
                        return &pft[i];
                }
        }
        return (proc_file *) NULL;
}

/**
 * Find a file in the global file table.
 *
 * @param fd    file descriptor
 * @return      pointer to the found file
 */

file* get_file(file_nr fd)
{
        for (int i = 0; i < NUM_FILES; i++) {
                if ( gft[i].f_desc == fd )
                        return &gft[i];
        }

        return (file *) NULL;
}

/**
 * Find a file in the process file table.
 *
 * @param pft   process file table
 * @param fd    file descriptor
 * @return      pointer to the file found
 */
proc_file* get_proc_file(proc_file pft[NUM_PROC_FILES], file_nr fd)
{
        for (int i = 0; i < NUM_PROC_FILES; i++) {
                if ( pft[i].pf_desc == fd )
                        return &pft[i];
        }

        return (proc_file *) NULL;
}

/**
 * Reset a file. Free memory if possible.
 *
 * @param fd    file descriptor
 */
void free_file(file_nr fd)
{
        file *f = get_file(fd);
        
        if (f != NULL){
                f->f_count--;
                if (f->f_count == 0) {
                        f->f_desc = NIL_FILE;
                        free(f->f_name);
                        f->f_name = (char *) NULL;
        
                        if (f->f_inode->i_adr != ROOT_INODE_BLOCK)
                                free_inode(f->f_inode->i_num);
                }
        }
}

/**
 * Reset a process file.
 *
 * @param pft   process file table
 * @param fd    the file
 */
void free_proc_file(proc_file pft[NUM_PROC_FILES], file_nr fd)
{
        proc_file *pf = get_proc_file(pft, fd);
        
        if(pf != NULL){
                pf->pf_desc = NIL_PROC_FILE;
                pf->pf_pos = 0;
                pf->pf_f_desc = NIL_FILE;
        }
}

/**
 * Increment the reference counter of a file.
 *
 * @param fd    file descriptor
 */
void inc_count(file_nr fd)
{
        file *f = get_file(fd);
        f->f_count++;
}

/**
 * Move file position to current file position + offset.
 *
 * @param pft    process file table
 * @param fd     file descriptor
 * @param offset offset
 * @param origin original position
 * @return       new position or -1 if failed
 */
size_t lseek(proc_file pft[NUM_PROC_FILES], file_nr fd, sint32 offset, uint32 origin)
{
        proc_file *pf = get_proc_file(pft, fd);
        
        if (pf == NULL){
                return -1;
        }
        
        pf->pf_pos = origin + offset;
        return pf->pf_pos;
}

/**
 * Find the descriptor of a file with the file name.
 *
 * @param name  file name
 * @return      found file descriptor
 */
file_nr name2desc(char *name) //in global file table
{
        for (int i = 0; i < NUM_FILES; i++) {
                //fs_dprintf("%s on 0x%d\n\n", gft[i].f_name, &gft[i].f_name);
                if (gft[i].f_name != (char *) NULL && strcmp(name, gft[i].f_name) == 0) {
                        fs_dprintf("[fs_file_table] %s and %s are equal -> return %d\n", name, gft[i].f_name, gft[i].f_desc);
                        return gft[i].f_desc;
                }
        }

        return NOT_FOUND;
}

/**
 * Find the descriptor of a file via its inode (number).
 *
 * @param  inode  the file's inode
 * @return        found file descriptor
 */
file_nr inode2desc(m_inode *inode)
{
        for (int i = 0; i < NUM_FILES; i++) {
                if ( inode->i_num == ((m_inode *)(gft[i].f_inode))->i_num ) {
                        return gft[i].f_desc;
                }
        }

        return NOT_FOUND;
}

/**
 * Look whether the global file table contains a special file descriptor.
 *
 * @param fd    file descriptor
 * @return      operation status
 */
bool contains_file(file_nr fd)
{
        return ( get_file(fd) != NULL);
}

/**
 * Check whether a file descriptors belong to a directory.
 *
 * @param fd    file descriptor
 * @return      belongs to a directory?
 */
bool is_directory(file_nr fd)
{
        return (get_file(fd)->f_inode)->i_mode == DIRECTORY;
}

/**
 * Get size of file/directory.
 * 
 * @param fd    file descriptor
 * @return      size
 */
size_t get_size(file_nr fd)
{
        return (get_file(fd)->f_inode)->i_size;
}

/********* DEBUG *********/

/**
 * Print out a file for debug purposes.
 *
 * @param f     file to be printed
 */
void dump_file(file *f)
{
        fs_dprintf("[fs_file_table] FILE: '%s': f_desc = %d; f_inode (nr)= %d; f_count = %d; f_mode = %d\n",
                   f->f_name, f->f_desc, (f->f_inode)->i_num, f->f_count, f->f_mode);
}

/**
 * Print out file table for debug purposes.
 */
void dump_files()
{
        for (int i = 0; i < NUM_FILES; i++) {
                dump_file(&gft[i]);
        }
}

/**
 * Print out a process file for debug purposes.
 *
 * @param pf     file to be printed
 */
void dump_proc_file(proc_file *pf)
{
        fs_dprintf("[fs_file_table] PROC_FILE: pf_desc = %d; pf_f_desc %d; pf_pos = %d\n",
                   pf->pf_desc, pf->pf_f_desc, pf->pf_pos);
}

/**
 * Print out process file table for debug purposes.
 */
void dump_proc_files(proc_file pft[NUM_PROC_FILES])
{
        for (int i = 0; i < NUM_PROC_FILES; i++) {
                dump_proc_file(&pft[i]);
        }
}
