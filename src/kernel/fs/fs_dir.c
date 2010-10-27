/* $Id$
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
 * Functions on directories.
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
#include "fs_dir.h"
#include "fs_block_dev.h"
#include "fs_bmap.h"
#include "fs_buf.h"
#include "fs_inode_table.h"
#include "fs_io_functions.h"

/**
 * Find a filename within a block of directory entries.
 * Format of an entry: (struct dir_entry): [block_nr, name].
 *
 * @param file_list  directory's file list of type dir_entry
 * @param name       file name which should be found
 * @return           block_nr of the file if found
 */
block_nr find_filename(dir_entry file_list[DIR_ENTRIES_PER_BLOCK], char *name)
{
        for (int i = 0; i < DIR_ENTRIES_PER_BLOCK; i++) {
                if (strcmp(file_list[i].name, name) == 0) {
                        fs_dprintf("[fs_dir] found file with inode on block %d, name = %s\n", file_list[i].inode, name);
                        return file_list[i].inode;
                }
        }
        return NOT_FOUND;
}

/**
 * Insert a new filename into a directory.
 *
 * @param dir_inode_blk block number of the directory inode
 * @param name          file name which should be inserted
 * @return              block number of the newly allocated inode block for 'name'
 */
block_nr insert_file_into_dir(block_nr dir_inode_blk, char *name)
{
        m_inode *dir_inode;             //the directory inode
        block_nr dir_entry_blk = 0;     //directory block with entries

        uint32 pos = 0;                 //position within directory
        uint32 insert_pos = NOT_FOUND;

        block_nr new_blk = 0;           //inode block for new file

        //initialize directory inode
        if (dir_inode_blk == ROOT_INODE_BLOCK) {
                dir_inode = root;
        } else {
                dir_inode = mallocn(sizeof(m_inode), "dir inode during creation process");
                read_minode(dir_inode, dir_inode_blk);

                if (dir_inode->i_mode != DIRECTORY) {
                        printf("%{ERROR: inode is no directory inode!}\n", RED);
                        return NOT_POSSIBLE;
                }
        }

        //scan the directory successively until a free entry is found
        do {
                dir_entry_blk = get_data_block(dir_inode, pos, TRUE);

                if (dir_entry_blk == NOT_POSSIBLE) {
                        return NOT_POSSIBLE;
                }

                if (fs_read(dir_cache, dir_inode, sizeof(dir_cache), pos, TRUE) == 0) {
                        return NOT_POSSIBLE; //problems during reading
                }

                if (contains_filename(dir_cache, name) == TRUE) { //file is already existent
                        return NOT_POSSIBLE;
                }

                new_blk = alloc_block(dir_entry_blk);
                insert_pos = create_entry(dir_cache, new_blk, name);

                if (insert_pos == NOT_FOUND) { //no free dir_entry
                        mark_block(new_blk, FALSE); //undo alloc_block()
                        pos += BLOCK_SIZE; //point to first byte of next block
                }

        } while (insert_pos == NOT_FOUND);

        dir_inode->i_size = MAX(dir_inode->i_size, pos + BLOCK_SIZE); // update inode size.
        dir_inode->i_modify_ts = time;
        write_inode(dir_inode); //write back modified directory inode

        fs_dprintf("[fs_dir] directory now has a size of %i blocks.\n", dir_inode->i_size/BLOCK_SIZE);

        wrt_block(dir_entry_blk, dir_cache, sizeof(dir_cache)); //write back modified dir_entry_block

        clear_block(new_blk); //reset new block
        fs_dprintf("[fs_dir] new block allocated for inode: %d\n", new_blk);

        if (dir_inode_blk != ROOT_INODE_BLOCK) {
                free(dir_inode);
        }

        return new_blk;
}

/**
 * Delete a new filename from a directory.
 *
 * @param dir_inode_blk block number of the directory inode
 * @param name          file name which should be deleted
 * @return              block number of deleted file's inode
 */
block_nr delete_file_from_dir(block_nr dir_inode_blk, char *name)
{
        m_inode *dir_inode;             //the directory inode
        block_nr dir_entry_blk = 0;     //directory block with entries

        uint32 pos = 0;                 //position within directory
        uint32 delete_pos = NOT_FOUND;


        if (dir_inode_blk == ROOT_INODE_BLOCK) {
                dir_inode = root;
        } else {
                dir_inode = mallocn(sizeof(m_inode), "dir inode during deletion process");
                read_minode(dir_inode, dir_inode_blk);

                if (dir_inode->i_mode != DIRECTORY) {
                        printf("%{ERROR: inode is no directory inode!}\n", RED);
                        return NOT_POSSIBLE;
                }
        }

        //scan the directory successively until the filename is found
        do {
                dir_entry_blk = get_data_block(dir_inode, pos, FALSE);

                if (dir_entry_blk == NOT_POSSIBLE) {
                        return NOT_POSSIBLE;
                }

                if (fs_read(dir_cache, dir_inode, sizeof(dir_cache), pos, FALSE) == 0) {
                        return NOT_POSSIBLE; //problems during reading
                }

                if (contains_filename(dir_cache, name)) {
                        // TODO: delete last block if it isn't used anymore,
                        //       stuff entries together if there's too much wasted space.
                        delete_pos = delete_entry(dir_cache, name);
                }

                if (delete_pos == NOT_FOUND) {
                        pos += BLOCK_SIZE; //to read the next block if not deleted successfully
                }

        } while (delete_pos == NOT_FOUND);

        wrt_block(dir_entry_blk, dir_cache, sizeof(dir_cache)); //write back modified dir_entry_block

        dir_inode->i_modify_ts = time;
        write_inode(dir_inode); //write back modified directory inode

        //fs_dprintf("[fs_dir] directory now contains %d files.\n", dir_inode->i_size);

        if (dir_inode_blk != ROOT_INODE_BLOCK) {
                free(dir_inode);
        }

        return delete_pos;
}

/**
 * Inserts a new file into a directory.
 *
 * @param file_list file list of type dir_entry
 * @param blk_nr    block number of the new file
 * @param name      name of the new file
 * @return          insert position
 */
uint32 create_entry(dir_entry file_list[DIR_ENTRIES_PER_BLOCK], block_nr blk_nr, char *name)
{
        for (int i = 0; i < DIR_ENTRIES_PER_BLOCK; i++) {
                if (strcmp(file_list[i].name, "") == 0) {
                        file_list[i].inode = blk_nr;
                        memcpy(file_list[i].name, name, NAME_SIZE);
                        return i*sizeof(dir_entry); //insert position
                }
        }
        return NOT_FOUND;
}

/**
 * Removes a file from a directory.
 *
 * @param file_list file list of type dir_entry
 * @param name      name of the file
 * @return          status of remove operation
 */
uint32 delete_entry(dir_entry file_list[DIR_ENTRIES_PER_BLOCK], char *name)
{
        for (int i = 0; i < DIR_ENTRIES_PER_BLOCK; i++) {
                if (strcmp(file_list[i].name, name) == 0) {
                        bzero(file_list[i].name, NAME_SIZE);
                        mark_block(file_list[i].inode, FALSE);
                        fs_dprintf("[fs_dir] marked inode block %d as FALSE\n", file_list[i].inode);
                        file_list[i].inode = NULL;

                        return i*sizeof(dir_entry); //delete position
                }
        }
        return NOT_POSSIBLE;
}

/**
 * Checks whether a directory contains a special filename
 *
 * @param file_list  directory's file list of type dir_entry
 * @param name       file name which should be found
 * @return           filename found?
 */
bool contains_filename(dir_entry file_list[DIR_ENTRIES_PER_BLOCK], char *name)
{
        return (find_filename(file_list, name) != NOT_FOUND);
}

/**
 * Prepare recursive search according to strtok() definition.
 *
 * @param path  absolute path to file
 * @return      block number of the file's inode (if found)
 */
block_nr search_file(char *path)
{
        char delim[] = "/";
        char *copy = strdup(path);
        char *work_copy = copy;

        char *tok = strsep(&work_copy, delim); //(should) delete "/" at the beginning to the path

        if (strcmp(tok, "") != 0) {
                fs_dprintf("[fs_dir] first token != NULL! ('%s')\n", tok);
                free(copy);
                return NOT_POSSIBLE; //wrong format
        }

        //search recursively
        block_nr ret = rfsearch(root->i_adr, work_copy, strsep(&work_copy, delim), delim);

        free(copy);
        return ret;
}

/**
 * Recursive search for files
 *
 * @param crt_dir       current directory to search in
 * @param path          absolute path to file
 * @param tok           a token of the path
 * @param delim         mostly '/'
 * @return              block number of the file's inode (if found)
 */
block_nr rfsearch(block_nr crt_dir, char *path, char *tok, char delim[])
{
        fs_dprintf("[fs_dir] searching for filename '%s' in block %d...\n", tok, crt_dir);

        m_inode *dir_inode;

        if (crt_dir == root->i_adr) {
                dir_inode = root;
        } else {
                read_minode(&m_inode_cache, crt_dir);
                dir_inode = &m_inode_cache;
        }

        uint32 pos = 0;
        uint32 bytes_read = 0;
        block_nr file_blk = NOT_FOUND; //corresponding block number to filename

        do {
                bytes_read = fs_read(dir_cache, dir_inode, sizeof(dir_cache), pos, FALSE); //read content = file list
                if (bytes_read != 0) { //otherwise dir_cache has not changed
                        file_blk = find_filename(dir_cache, tok);  //find current file in the directory cache
                }
                pos += BLOCK_SIZE; //next data block with dir_entries

        } while (file_blk == NOT_FOUND && bytes_read != 0);

        if (file_blk == NOT_FOUND) {
                fs_dprintf("[fs_dir] file not found\n");
                return NOT_FOUND;
        }

        tok = strsep(&path, delim);

        if (tok == NULL) { //end of path
                return file_blk;
        } else {
                return rfsearch(file_blk, path, tok, delim);
        }
}

/**
 * Extract the filename WITHOUT the path from the absolute path.
 *
 * @param abs_path absolute path to file
 * @return         only the filename
 */
char* get_filename(char *abs_path)
{
        char delim = '/';
        char *path = mallocn(strlen(abs_path), "path in get_filename()");

        if (path == NULL) {
                return NULL;
        }

        bzero(path, strlen(abs_path));

        int i = strlen(abs_path);

        while (abs_path[--i] != delim);

        memcpy(path, abs_path + i + 1, strlen(abs_path) - i);

        fs_dprintf("[fs_dir] extracted filename = %s\n", path);
        return path;
}

/**
 * Extract the path WITHOUT the filename from the absolute path.
 *
 * @param abs_path absolute path to file
 * @return         only the path
 */
char* get_path(char *abs_path)
{
        char delim = '/';
        char *path = mallocn(strlen(abs_path), "path in get_path()");//, "extract path from abs_path");

        if (path == NULL) {
                return NULL;
        }

        bzero(path, strlen(abs_path));

        int i = strlen(abs_path);

        while (abs_path[--i] != delim);
        if (i == 0) i++; //get at least '/'

        memcpy(path, abs_path, i);

        fs_dprintf("[fs_dir] extracted path = %s\n", path);

        return path;
}
