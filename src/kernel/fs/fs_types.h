/* $Id: fs_types.h 266 2009-10-14 08:15:36Z dtraytel $
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
 * Basic type definitions. This file defines basic data types used
 * throughout the file system. You should use these types whenever possible
 * in order to avoid the possible ambiguity of the builtin types.
 *
 * @author Vincenz Doelle
 * @author $LastChangedBy: dtraytel $
 * @version $Rev: 266 $
 */

#ifndef __FS_TYPES_H_
#define __FS_TYPES_H_

#include "fs_const.h"
#include "../io/io_rtc.h"

typedef uint32 block_nr;
typedef sint16 inode_nr;
typedef sint16 file_nr;


/**
 * The directory entry.
 */
typedef struct dir_entry {
        block_nr inode;
        char name[NAME_SIZE];
} __attribute__((packed)) dir_entry;


/**
 * The inode on disk.
 */
typedef struct d_inode {
        uint16 i_mode;                          /* file | directory */
        uint32 i_size;                          /* in byte */
        time_t i_create_ts;
        time_t i_modify_ts;
        block_nr i_direct_pointer[NUM_DIRECT_POINTER];
        block_nr i_single_indirect_pointer;
        block_nr i_double_indirect_pointer;
} __attribute__((packed)) d_inode;


/**
 * The inode in memory.
 */
typedef struct m_inode {
        inode_nr i_num;
        block_nr i_adr;
        uint16 i_mode;
        uint32 i_size;
        time_t i_create_ts;
        time_t i_modify_ts;
        block_nr i_direct_pointer[NUM_DIRECT_POINTER];
        block_nr i_single_indirect_pointer;
        block_nr i_double_indirect_pointer;
} m_inode;


/**
 * The global file descriptor.
 */
typedef struct file {
        file_nr f_desc;                         /* file (f) descriptor */
        m_inode *f_inode;
        char *f_name;                           /* pointer to absolute file path */
        uint8 f_mode;                           /* see i_mode */
        uint16 f_count;                         /* number of process file descriptors accessing this file */
} file;

/**
 * The process file descriptor.
 */
typedef struct proc_file {
        file_nr pf_desc;                        /* process file (pf) descriptor */
        file_nr pf_f_desc;                      /* pointer to global file descriptor */
        uint32 pf_pos;                          /* position in file */
} proc_file;                                    /* process file descriptor from process filp */


/**
 * A buffer for one block.
 */
typedef struct block_buffer {
        block_nr block_nr;
        uint8 cache[BLOCK_SIZE];                /* BLOCK_SIZE * 1 byte = BLOCK_SIZE byte space for block content */
} block_buffer;


/**
 * The cache for one generic block.
 */
typedef block_buffer block_cache;


/*
 * A struct containing file information.
 */
typedef struct file_info {
        char name[NAME_SIZE];
        uint16 mode;
        uint32 size;
        time_t create_ts;
        time_t modify_ts;
        uint16 num_links;
} file_info_t;



#endif /*__FS_TYPES_H_*/
