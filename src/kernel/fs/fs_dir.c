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
 * @param file_list The directory's file list of type dir_entry
 * @param name      The file name which should be found
 * @return          The block_nr of the file if found
 */
block_nr find_filename(dir_entry file_list[DIR_ENTRIES_PER_BLOCK], char *name)
{
        for (int i = 0; i < DIR_ENTRIES_PER_BLOCK; i++){
                if(strcmp(name, file_list[i].name) == 0){
                        return file_list[i].inode;
                }
        }
        return NOT_FOUND;
}

block_nr insert_file_into_dir(block_nr dir_inode_blk, char *name)
{
        bool inserted = FALSE;
        uint32 pos = 0; //points to start of block 0, 1, 2, ...
        block_nr dir_entry_blk = 0;
        block_nr new_blk = 0;
        
        m_inode *dir_inode = malloc(sizeof(m_inode)); //for fs_read()
        read_minode(dir_inode, dir_inode_blk);
        
        //scan the directory successively until a free entry is found 
        do{
                dir_entry_blk = fs_read(dir_cache, dir_inode, sizeof(dir_cache), pos);
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
        
        wrt_block(dir_entry_blk, dir_cache, sizeof(dir_cache)); //write back modified dir_entry_block
        
        clear_block(new_blk); //reset new block
        
        return new_blk;
}

bool insert_filename(dir_entry file_list[DIR_ENTRIES_PER_BLOCK], block_nr blk_nr, char *name)
{
        for (int i = 0; i < DIR_ENTRIES_PER_BLOCK; i++){
                if (strcmp(file_list[i].name, "") == 0){
                        file_list[i].inode = blk_nr;
                        memcpy(file_list[i].name, name, NAME_SIZE);
                        return TRUE;
                }
        }
        return FALSE;
}

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
 * @param path  The absolute file path
 * @return      The block number of the file's inode (if found)
 */
block_nr search_file(char *path)
{
        char delim[] = "/";
        char *copy = strdup(path);
        char *work_copy = copy;
        
        char *tok = strsep(&work_copy, delim); //(should) delete "/" at the beginning to the path
        if (tok != NULL){
                return NOT_POSSIBLE; //wrong format
        }
        
        return rfsearch(root->i_adr, work_copy, strsep(&work_copy, delim), delim);
        
}

/**
 * recursive search for files
 * 
 * @param path The absolute file path
 * @return The block number of the file's inode (if found)
 */
block_nr rfsearch(block_nr crt_dir, char *path, char *tok, char delim[])
{
        uint32 pos = 0;
        rd_block(&d_inode_cache, crt_dir, BLOCK_SIZE); //read d_inode
        block_nr read;
        
        do{
                cpy_dinode_to_minode(&m_inode_cache, &d_inode_cache); //convert d_inode to m_inode (for fs_read)
                m_inode_cache.i_adr = crt_dir;
                
                read = fs_read(dir_cache, &m_inode_cache, sizeof(dir_cache), pos); //read content = file list
                
                //pos += (ADDR_SIZE + NAME_SIZE) / 4;     //next dir_entry
                pos += BLOCK_SIZE; //next data block with dir_entrys //TODO: pos += ?
                
        } while(contains_filename(dir_cache, tok) == FALSE && read != NOT_POSSIBLE);
                
                
        block_nr blk_nr = find_filename(dir_cache, tok); //find current file in the directory cache
        if (blk_nr == NOT_FOUND){
                return NOT_FOUND;
        }
        
        //rd_block(dir_cache, blk_nr, sizeof(dir_cache)); //read blk_nr to directory cache
        
        tok = strsep(&path, delim);
        
        if (tok == NULL){ //end of path
                return blk_nr;
        } else {
                return rfsearch(blk_nr, path, tok, delim);
        }
}

/**
 * Extract the filename WITHOUT the path from the absolute path.
 * 
 * @param abs_path The absolute path
 * @return         The filename
 */
char* get_filename(char *abs_path)
{
        char delim[] = "/";
        char *tok;
        char *copy = strdup(abs_path);
        char *work_copy = copy;
        char *backup = malloc(NAME_SIZE);
        char *pbackup = backup;
        
        do {
                if (tok != NULL){
                         bzero(backup, NAME_SIZE);       //reset string 
                         backup = strdup(tok);           //backup tok
                }
                tok = strsep(&work_copy, delim);
        } while (tok != NULL);
        
        return pbackup;
}


/**
 * Extract the path WITHOUT the filename from the absolute path.
 * 
 * @param abs_path The absolute path
 * @return         The path
 */
char* get_path(char *abs_path)
{
        char delim[] = "/";
        char *tok;
        char *copy = strdup(abs_path);
        char *work_copy = copy;
        char *path = malloc(sizeof(*abs_path));
        char *ppath = path;
        
        path = "/";
        
        do {
                if (tok != NULL){
                        memcpy(path, tok, sizeof(tok));
                        path = "/";
                }
                tok = strsep(&work_copy, delim);
        } while (tok != NULL);
        
        return ppath;
}
