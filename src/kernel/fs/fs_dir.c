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
 * Functions on directorys.
 *
 * @author Vincenz Doelle
 * @author $LastChangedBy$
 * @version $Rev$
 */

/* char* strsep Example:
 *      char path[] = "/usr/share/bin/editor";<p>
 *      char delim[] = "/";<p>
 *      char *tok;<p>
 *      char *copy = strdup(path);<p>
 *      char *work_copy = copy;<p><p>
 *
 *      do {<p>
 *              printf("strsep(\"%s\") ", work_copy);<p>
 *              tok = strsep(&work_copy, delim);<p>
 *              printf("-> \"%s\"\n", tok);<p>
 *      } while (tok != NULL);<p><p>
 *
 *      printf("\ncopy = %p\n", copy);<p>
 *      printf("work_copy = %p\n", work_copy);<p>
 *      puts("done.");<p>
 *
 *      free(copy);
 */
#include "../include/const.h"
#include "../include/types.h"
#include "../include/string.h"
#include "../include/stdlib.h"
#include "../include/debug.h"

#include "fs_const.h"
#include "fs_types.h"
#include "fs_block_dev.h"
#include "fs_bmap.h"
#include "fs_buf.h"
#include "fs_inode_table.h"
#include "fs_dir.h"
#include "fs_io_functions.h"

/**
 * Find a filename within a block of directory entries; format(struct dir_entry): [block_nr, name].
 * 
 * @param file_list directory's file list of type dir_entry
 * @param name      file name which should be found
 * @return          block_nr of the file if found
 */
block_nr find_filename(dir_entry file_list[DIR_ENTRIES_PER_BLOCK], char *name)
{
        for (int i = 0; i < DIR_ENTRIES_PER_BLOCK; i++){
                //dprintf("[fs_dir] strcmp(%s,%s)\n", file_list[i].name, name);
                if(strcmp(file_list[i].name, name) == 0){
//                        dprintf("[fs_dir] found file with inode on block %d\n", file_list[i].inode);
                        return file_list[i].inode;
                }
        }
        return NOT_FOUND;
}

/**
 * Inserts a new filename into a directory.
 * 
 * @param dir_inode_blk block number of the directory's inode
 * @param name          file name which should be inserted
 * @return              number of the newly allocated inode block for 'filename'
 */  
block_nr insert_file_into_dir(block_nr dir_inode_blk, char *name)
{
        bool inserted = FALSE;
        uint32 pos = 0; //points to start of block 0, 1, 2, ...
        block_nr dir_entry_blk = 0;
        block_nr new_blk = 0;
        
        m_inode *dir_inode;
        
        if (dir_inode_blk == ROOT_INODE_BLOCK){
                dir_inode = root; 
        } else {
                dir_inode = malloc(sizeof(m_inode)); //for fs_read()
                read_minode(dir_inode, dir_inode_blk);
                
                /*if (dir_inode->i_mode != DIRECTORY){
                        return NOT_POSSIBLE;
                }*/
        }
        
        //scan the directory successively until a free entry is found 
        do{
                dir_entry_blk = fs_read(dir_cache, dir_inode, sizeof(dir_cache), pos, TRUE);
                
//                dprintf("[fs_dir] dir_entry_blk = %d\n", dir_entry_blk);
                
                if (dir_entry_blk == NOT_POSSIBLE){
                        return NOT_POSSIBLE; //problems during reading
                }
                
                if (contains_filename(dir_cache, name) == TRUE){
                        return NOT_POSSIBLE; //file is already existent
                }
                
                new_blk = alloc_block(dir_entry_blk);
                inserted = insert_filename(dir_cache, new_blk, name);
                
                if (inserted == FALSE){ //no free dir_entry
                        mark_block(new_blk, FALSE);
                        pos += BLOCK_SIZE; //to read the next block if not inserted successfully
                }

        } while(inserted == FALSE);
        
//        dprintf("[fs_dir] wrt_block(%d, dir_cache, %d)\n", dir_entry_blk, sizeof(dir_cache));
        
        wrt_block(dir_entry_blk, dir_cache, sizeof(dir_cache)); //write back modified dir_entry_block
        
        clear_block(new_blk); //reset new block
        
//        dprintf("[fs_dir] new_blk = %d\n", new_blk);
        
        free(dir_inode);
        return new_blk;
}

/**
 * Inserts a new file into a directory.
 * 
 * @param file_list file list of type dir_entry
 * @param blk_nr    block number of the new file
 * @param name      name of the new file
 * @return          status of insert operation
 */    
bool insert_filename(dir_entry file_list[DIR_ENTRIES_PER_BLOCK], block_nr blk_nr, char *name)
{
        for (int i = 0; i < DIR_ENTRIES_PER_BLOCK; i++){
                if (strcmp(file_list[i].name, "") == 0){
//                        dprintf("[fs_dir] found empty file list entry at %d!\n", i);
                        
                        file_list[i].inode = blk_nr;
                        memcpy(file_list[i].name, name, NAME_SIZE);
                        return TRUE;
                }
        }
        return FALSE;
}

/**
 * Removes a file from a directory.
 * 
 * @param file_list file list of type dir_entry
 * @param name      name of the file
 * @return          status of remove operation
 */    
bool remove_filename(dir_entry file_list[DIR_ENTRIES_PER_BLOCK], char *name)
{
        for (int i = 0; i < DIR_ENTRIES_PER_BLOCK; i++){
                if (strcmp(file_list[i].name, name) == 0){
                        bzero(file_list[i].name, NAME_SIZE);
                        file_list[i].inode = NULL;
                }
        }

}

/**
 * Checks whether a directory contains a special filename
 * 
 * @param see find_file()
 * @return TRUE | FALSE
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

//        dprintf("[fs_dir] '%s' = strsep('%s', '/')\n", tok, path);
        if (strcmp(tok, "") != 0){
                dprintf("[fs_dir] first token != NULL! ('%s')\n", tok);
                return NOT_POSSIBLE; //wrong format
        }
        
        return rfsearch(root->i_adr, work_copy, strsep(&work_copy, delim), delim);
        
}

/**
 * recursive search for files
 * 
 * @param path  absolute path to file
 * @return      block number of the file's inode (if found)
 */
block_nr rfsearch(block_nr crt_dir, char *path, char *tok, char delim[])
{
        dprintf("\n[fs_dir] processing path = '%s' | tok = '%s'\n", path, tok);
        dprintf("[fs_dir] searching for filename '%s' in block %d...\n", tok, crt_dir);
        
        uint32 pos = 0;
        block_nr read;
        block_nr file_blk;
        m_inode *dir_inode;
        
        if (crt_dir != root->i_adr){
                dprintf("reading dir_inode from block %d: \n", crt_dir);
                read_minode(&m_inode_cache, crt_dir);
                dir_inode = &m_inode_cache;
        } else {
                dprintf("dir_inode is root!\n");
                dir_inode = root;
        }
        
        dprintf("dir_inode in block %d:\n", crt_dir);
        
        do{
                read = fs_read(dir_cache, dir_inode, sizeof(dir_cache), pos, FALSE); //read content = file list
                pos += BLOCK_SIZE; //next data block with dir_entrys //TODO: pos += ?
                file_blk = find_filename(dir_cache, tok);  //find current file in the directory cache
                
        } while(file_blk == NOT_FOUND && read != NOT_POSSIBLE);
        
        if (file_blk == NOT_FOUND){
                dprintf("[fs_dir]file not found\n");
                return NOT_FOUND;
        }
        
        tok = strsep(&path, delim);
        
        if (tok == NULL){ //end of path
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
        char *path = malloc(strlen(abs_path));
        if (path == (void*) NULL) return (char*) NULL;
        bzero(path, strlen(abs_path));
        
        int i = strlen(abs_path);
              
        while(abs_path[--i] != delim);
        
        memcpy(path, abs_path+i+1, strlen(abs_path)-i);
        
        printf("[fs_dir] extracted filename = %s\n", path);
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
        char *path = malloc(strlen(abs_path));//, "extract path from abs_path");
        
        if (path == (void*) NULL) return (char*) NULL;
        
        bzero(path, strlen(abs_path));
        
        int i = strlen(abs_path);
              
        while(abs_path[--i] != delim);
        if (i == 0) i++; //get at least '/'
        
        memcpy(path, abs_path, i);
        
        dprintf("[fs_dir] extracted path = %s\n", path);

        return path;
}
