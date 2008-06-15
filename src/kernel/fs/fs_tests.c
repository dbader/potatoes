#include "../include/const.h"
#include "../include/types.h"
#include "../include/string.h"
#include "../include/debug.h"
#include "../include/assert.h"

#include "fs_const.h"
#include "fs_types.h"
#include "fs_file_table.h"
#include "fs_inode_table.h"
#include "fs_io_functions.h"
#include "fs_super.h"
#include "fs_bmap.h"
#include "fs_main.h"
#include "fs_buf.h"



void test_bmap() /* [successful] */
{
        dprintf("allocated 10?: %d\n", is_allocated_block(10));
        dprintf("allocated 42?: %d\n", is_allocated_block(42));
        dprintf("first free %d\n", get_free_block(FIRST_DATA_BLOCK));
        dprintf("alloc new: %d\n", alloc_block(FIRST_DATA_BLOCK));
        dprintf("first free %d\n", get_free_block(FIRST_DATA_BLOCK));
        mark_block(42, TRUE);
        dprintf("allocated 42?: %d\n", is_allocated_block(42));
        mark_block(10, FALSE);
        dump_bmap();
}

void test_inode_table() /* [successful] */
{
        dump_inodes();
                
        m_inode *mi = alloc_inode();
        dprintf("allocacted inode: %d\n", mi->i_num);
        dump_inodes();
                
        dprintf("free inode %d", mi->i_num);
        free_inode(mi->i_num);
        dprintf("done\n");
        dump_inodes();
}

void test_file_table() /* [successful] */
{
        init_file_table();
        m_inode *mi = new_minode(4242, 1, TRUE);
        m_inode *mi2 = new_minode(4343, 1, TRUE);
        m_inode *mi3 = new_minode(4444, 1, FALSE);
                
        dump_inodes();
        dump_files();
        
        char *name = "/dir1/file11.txt";
        char *name2 = "/dir1/file12.txt";
        if (insert_file(mi, name, DATA_FILE) == NOT_POSSIBLE) dprintf("insertion not possible!\n");
        if (insert_file(mi2, name2, DIRECTORY) == NOT_POSSIBLE) dprintf("insertion2 not possible!\n");
        
        dump_files();
        
        dprintf("file_table contains FD = 11? : %d\n", contains_file(11));
        dprintf("file_table contains FD = 12? : %d\n", contains_file(12));
        dprintf("file_table contains FD = 110? : %d\n", contains_file(110));
        
        dprintf("FD of root: %d\n", inode2desc(root));
        dprintf("FD of mi: %d\n", inode2desc(mi));
        dprintf("FD of %s: %d\n", name, name2desc(name));
        
        char *false_name = "/dir1/false";
        dprintf("FD of %s: %d\n", false_name, name2desc(false_name));
        
        free_file(11);
        dump_files();
                
}

void test_reload()
{
        /* [successful]
        dprintf("\nreloading super:\n\n");
        test_bmap();
        dump_bmap();
        write_bmap();
        load_bmap();
        dump_bmap();
        */
        
        /* [successful]
        dprintf("\nreloading super:\n\n");
        write_super_block();
        dump_super();
        load_super_block();
        dump_super();
        */
        
        /* [successful]
        dprintf("\nreloading root:\n\n");
        root->i_single_indirect_pointer = 42;
        root->i_double_indirect_pointer = 4242;
        write_root();
        dump_inode(ROOT_INODE);
        bzero(&inode_table[ROOT_INODE], sizeof(m_inode));
        load_root();
        dump_inode(ROOT_INODE);
        */
}

void test_create()
{
        if (!fs_create("/dir1", DIRECTORY)) dprintf("creation unsuccessful!\n");
        if (!fs_create("/dir1/dir2", DIRECTORY)) dprintf("creation unsuccessful!\n");
        if (!fs_create("/dir1/dir2/dir3", DIRECTORY)) dprintf("creation unsuccessful!\n");
        if (!fs_create("/dir1/dir4", DIRECTORY)) dprintf("creation unsuccessful!\n");
                
        dprintf("\n");
        
        dump_inode(&inode_table[ROOT_INODE]);
        dprintf("\n");
        //if (!fs_create("/dir1/dir2", DIRECTORY)) dprintf("creation unsuccessful!\n");

        dprintf("\ncontent of dir_entry_block 34:\n");
        char *p1 = malloc_clean(BLOCK_SIZE);
        rd_block(p1, 34, BLOCK_SIZE);
        for (int i = 0; i < BLOCK_SIZE; i++){
                dprintf("%s", p1++);
                
                if (i% sizeof(dir_entry) == 0)
                        dprintf("\n");
        }
        
        /*dump_inode(root);
        write_inode(root);
        
        read_minode(root, 33);
        dump_inode(root);
        */
        
        //if (fs_open("/dir1") == -1) dprintf("opening unsuccessful!\n");
        //dump_inodes();
        //dump_files();
}

void test_read()
{
        
}

void run_FS_tests()
{
        test_create();
}
