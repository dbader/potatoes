/* $Id: inode_table.h 37 2008-05-02 22:59:03Z vdoelle $
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
 * The inode table.
 * This table contains the currently opened inodes.
 * This table is only existent in memory.
 *
 * @author Vincenz Doelle
 * @author $LastChangedBy: vdoelle $
 * @version $Rev: 37 $
 */

#ifndef INODE_H_
#define INODE_H_

/* central struct */

extern m_inode inode_table[NUM_INODES];


/* definition */

#define NIL_INODE (m_inode *) 0                         /* there is no free inode entry left */


/* functions */

extern m_inode * read_inode(inode_nr inode);

extern uint8 write_inode(m_inode *inode);

extern uint8 free_inode(inode_nr inode);

extern uint8 alloc_inode(void);

extern uint32 new_inode(); 

#endif /*INODE_H_*/
