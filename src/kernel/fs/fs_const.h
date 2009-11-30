/* $Id: fs_const.h 266 2009-10-14 08:15:36Z dtraytel $
********************************************************************************
* _____   ____ _______    _______ ____  ______  _____                          *
*|  __ \ / __ \__   __|/\|__   __/ __ \|  ____|/ ____|          Copyright 2008 *
*| |__) | |  | | | |  /  \  | | | |  | | |__  | (___              Daniel Bader *
*|  ___/| |  | | | | / /\ \ | | | |  | |  __|  \___ \           Vincenz Doelle *
*| |    | |__| | | |/ ____ \| | | |__| | |____ ____) |    Johannes Schamburger *
*|_|     \____/  |_/_/    \_\_|  \____/|______|_____/          Dmitriy Traytel *
*                                                                              *
*      Practical Oriented TeAching Tool, Operating (and) Educating System      *
*                                                                              *
*                           www.potatoes-project.tk                            *
*******************************************************************************/

/**
 * @file
 * Basic constant definitions. This file defines constants used
 * throughout the file system.
 *
 * @author Vincenz Doelle
 * @author $LastChangedBy: dtraytel $
 * @version $Rev: 266 $
 */


#ifndef __FS_CONST_H_
#define __FS_CONST_H_


#define BLOCK_SIZE              512
#define NAME_SIZE               28
#define DIR_ENTRY_SIZE          sizeof (struct dir_entry)               //4 + NAME_SIZE
#define DISK_INODE_SIZE         sizeof (struct d_inode)                 //2 + 5*4 + NUM_DIRECT_POINTER*4 
#define MEM_INODE_SIZE          sizeof (struct m_inode)                 //2*2 + 6*4 + NUM_DIRECT_POINTER*4 
#define SUPER_SIZE              sizeof (struct super_block)

#define INODES_PER_BLOCK        1
#define DIR_ENTRIES_PER_BLOCK   ((BLOCK_SIZE)/(DIR_ENTRY_SIZE))

#define NUM_FILES               10                                      /* 64; #entries in filp table */
#define NUM_PROC_FILES          20                                      /* #concurrently opened files per process */
#define NUM_INODES              10                                      /* 200; max. num of opened inodes(files+dir) */

#define ADDR_SIZE               4
#define ADDRS_PER_BLOCK         ((BLOCK_SIZE)/(ADDR_SIZE))
#define NUM_DIRECT_POINTER      30                                      /* # direct pointer in inode */
#define BYTES_DIRECT            ((NUM_DIRECT_POINTER) * (BLOCK_SIZE))   /* # bytes accessable by direct pointer in inode */
#define BYTES_SINGLE_INDIRECT   ((ADDRS_PER_BLOCK) * (BLOCK_SIZE))      /* # bytes accessable by single indirect pointer in inode */
#define BYTES_DOUBLE_INDIRECT   ((ADDRS_PER_BLOCK) * (ADDRS_PER_BLOCK) * (BLOCK_SIZE)) /* # bytes accessable by double indirect pointer in inode */

#define BOOT_BLOCK              ((block_nr) 0)
#define SUPER_BLOCK             ((block_nr) 1)
#define ROOT_INODE              ((inode_nr) 0)
#define ROOT_INODE_BLOCK        ((block_nr) 2)
#define FIRST_BMAP_BLOCK        ((block_nr) 3)

#define DATA_FILE               1
#define DIRECTORY               2

#define CLEAN                   0
#define DIRTY                   1

#define NOT_EXISTENT            -1
#define NOT_FOUND               -1
#define NOT_POSSIBLE            -1

#define MAGIC_NUMBER            280187

#endif /*__FS_CONST_H_*/
