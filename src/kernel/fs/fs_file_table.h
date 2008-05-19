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
sint16 insert_file(m_inode *inode, char *name, uint8 mode);
sint16 insert_proc_file(proc_file proc_filp[], uint16 glo_desc);
sint16 alloc_file();
sint16 alloc_proc_file(proc_file proc_filp[]);
file *get_file(uint16 fd);
void inc_count(uint16 fd);
sint16 name2desc(char *name);
sint16 inode2desc(m_inode *inode);
bool contains_file(uint16 fd);
uint16 conv_desc(uint16 desc);

/* definition */

#define NIL_FILE 0

#define NIL_PROC_FILE 0


#endif /*__FS_FILE_TABLE_H_*/
