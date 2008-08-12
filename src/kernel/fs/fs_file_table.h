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
 * The file descriptor table.
 *
 * @author Vincenz Doelle
 * @author $LastChangedBy$
 * @version $Rev$
 */

#ifndef __FS_FILE_TABLE_H_
#define __FS_FILE_TABLE_H_

/* the global file table (gft) */
file gft[NUM_FILES];


/* functions */

void init_file_table();
void init_proc_file_table(proc_file proc_filp[]);

void dump_file(file *f);
void dump_files();
void dump_proc_file(proc_file *pf);
void dump_proc_files();

file_nr insert_file(m_inode *inode, char *name, uint8 mode);
file_nr insert_proc_file(proc_file pft[NUM_PROC_FILES], file_nr glo_fd);

file* alloc_file();
proc_file* alloc_proc_file(proc_file pft[NUM_PROC_FILES]);

file* get_file(file_nr fd);
proc_file* get_proc_file(proc_file pft[NUM_PROC_FILES], file_nr fd);

void free_file(file_nr fd);
void free_proc_file(proc_file pft[NUM_PROC_FILES], file_nr fd);

bool contains_file(file_nr fd);
bool is_directory(file_nr fd);

file_nr name2desc(char *name);
file_nr inode2desc(m_inode *inode);

void inc_count(file_nr fd);
void lseek(proc_file pft[NUM_PROC_FILES], file_nr fd, sint32 offset, uint32 origin);

/* definition */

#define NIL_FILE -1

#define NIL_PROC_FILE -1


#endif /*__FS_FILE_TABLE_H_*/
