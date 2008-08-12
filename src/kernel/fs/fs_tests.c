#include "../include/const.h"
#include "../include/types.h"
#include "../include/string.h"
#include "../include/debug.h"
#include "../include/stdlib.h"
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
#include "fs_block_dev.h"



void test_bmap() /* [successful] */
{
        dprintf("allocated 10?: %d\n", is_allocated_block(10));
        dprintf("allocated 42?: %d\n", is_allocated_block(42));
        dprintf("first free %d\n", get_free_block(super_block.s_first_data_block));
        dprintf("alloc new: %d\n", alloc_block(super_block.s_first_data_block));
        dprintf("first free %d\n", get_free_block(super_block.s_first_data_block));
        mark_block(42, TRUE);
        dprintf("allocated 42?: %d\n", is_allocated_block(42));
        mark_block(42, FALSE);
        mark_block(100, TRUE);
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

void __ls(char* path){
        dir_entry directory [DIR_ENTRIES_PER_BLOCK];
        bzero(directory, sizeof(directory));
        
        file_nr fd = do_open(path);
        dprintf("%d bytes read. \n", do_read(fd, directory, sizeof(directory), 0));
               
        dprintf("inode\tname\n-------------------------------------\n");
        int i = 0;
        while (directory[i].inode != 0) {
                dprintf("%d\t%s\n", directory[i].inode, directory[i].name);
                i++;
        }
        do_close(fd);
}

void test_create()
{
        
        char number[4];
        char buf[5];
        for (int i = 1; i < 10; i++){
                bzero(number, 4);
                itoa(i, number, 10);
                bzero(buf, 5);
                strcat(buf, "/dir");
                strcat(buf, number);
                //dprintf("%\n", strcat("dir", "1"));
                if (!fs_create(buf, DIRECTORY)) dprintf("creation unsuccessful!\n");
        }
        
        
        //if (!fs_create("/dir1", DIRECTORY)) dprintf("creation unsuccessful!\n");
        //if (!fs_create("/dir1/dir2/dir3", DIRECTORY)) dprintf("creation unsuccessful!\n");
        //if (!fs_create("/dir1/dir4", DIRECTORY)) dprintf("creation unsuccessful!\n");
                
        dprintf("\n");
        
        __ls("/dir1");
}

extern void mem_dump();

void test_rw_quantitative() //TODO!
{
        char to_write[30] = "Hallo, ich war auf der HDD!";
        if (!fs_create("/file1", DATA_FILE)) {
                dprintf("creation unsuccessful!\n");
                return;
        }
        
        char to_read[100];
        bzero(to_read, sizeof(to_read));
        
        if (fs_open("/file1") == NOT_POSSIBLE){
                dprintf("opening unsuccessful!\n");
                return;
        }
        
        int num_bytes = 0;
        
        file *f = get_file(name2desc("/file1"));
        m_inode *mi = f->f_inode;
        for (int pos = 0; pos < 1000 * sizeof(to_write); pos += sizeof(to_write)){
                num_bytes = fs_write(mi, to_write, sizeof(to_write), pos, TRUE);
                dprintf("wrote %d bytes\n", num_bytes);
                if (num_bytes == 0){
                        dprintf("writing unsuccessful!\n");
                        return;
                }
                dprintf("\n");
        }
        
        mem_dump();
        
        dprintf("written: %s\n\n", to_write);
        
        num_bytes = fs_read(to_read, mi, sizeof(to_read), 0, FALSE);
        dprintf("read %d bytes\n", num_bytes);
        if (num_bytes == 0){
                dprintf("reading unsuccessful!\n");
                return;   
        }
        
        dprintf("read: %s\n", to_read);
}

void test_rw_qualitative()
{
        char string[30] = "Hallo, ich war auf der HDD!!!!"; 
        char to_write[60];
        
        for (int i = 0; i < 2; i++){
                strcat(to_write, string);
        }

        if (!fs_create("/file1", DATA_FILE)) {
                dprintf("creation unsuccessful!\n");
                return;
        }


        if (fs_open("/file1") == NOT_POSSIBLE) {
                dprintf("opening unsuccessful!\n");
                return;
        }

        m_inode *mi = get_file(name2desc("/file1"))->f_inode;
        int num_bytes = fs_write(mi, to_write, sizeof(to_write), 0, TRUE);
        dprintf("wrote %d bytes.\n", num_bytes);
        if (num_bytes == 0) {
                dprintf("writing unsuccessful!\n");
                return;
        }

        char to_read[1000];
        bzero(to_read, sizeof(to_read));

        num_bytes = fs_read(to_read, mi, sizeof(to_read), 0, FALSE);
        if (num_bytes == 0){
                dprintf("reading unsuccessful!\n");
                return;   
        } else {
                dprintf("read %d bytes: %s[END]\n", num_bytes, to_read);
        }
        
        //write again with offset 7
        if (fs_write(mi, to_write, sizeof(to_write), 7, TRUE) == 0) {
                dprintf("writing unsuccessful!\n");
                return;
        }
        
        bzero(to_read, sizeof(to_read));

        num_bytes = fs_read(to_read, mi, sizeof(to_read), 0, FALSE);
        if (num_bytes == 0){
                dprintf("reading unsuccessful!\n");
                return;   
        } else {
                dprintf("read %d bytes: %s[END]\n", num_bytes, to_read);
        }
}

void test_PM()
{
        proc_file pft[NUM_PROC_FILES];
        init_proc_file_table(pft);
        
        int fd = -1;
        
        // mkdir dir
        dprintf("\n\n--- mkdir dir\n");
        do_mkdir("/dir");
        fd = do_open("/dir");
        fd = insert_proc_file(pft, fd);
        do_close_pf(pft, fd);
        
        //cd dir
        dprintf("\n\n--- cd dir\n");
        fd = do_open("/dir");
        fd = insert_proc_file(pft, fd);
        do_close_pf(pft, fd);
        
        //touch file
        dprintf("\n\n--- touch file\n");
        fd = do_create("/dir/file", 0);
        fd = insert_proc_file(pft, fd);
        do_close_pf(pft, fd);
        
        //ls
        dprintf("\n\n--- ls\n");
        fd = do_open("/dir");
        fd = insert_proc_file(pft, fd);
        
        dir_entry directory [DIR_ENTRIES_PER_BLOCK];
        bzero(directory, sizeof(directory));
        
        proc_file *pft_entry = get_proc_file(pft, fd);
        do_read(pft_entry->pf_f_desc, directory, sizeof(directory), 0);        
        do_close_pf(pft, fd);
        
        /*
        do_mkfile("/usr/myfile");
        
        file_nr fd = do_open("/usr/myfile");       
        file_nr pfd = insert_proc_file(pft, fd);
        
        //dump_files();
        //dump_proc_files();
        
        char to_write[20] = "Hallo 42!";
        do_write(fd, to_write, sizeof(to_write), 0);
        
        char to_read[20];
        bzero(to_read, sizeof(to_read));
        
        do_read(fd, to_read, sizeof(to_read), 6);
        
        dprintf("%s\n", to_read);
        */
}

void test_error()
{
        if (!do_mkdir("/usr")) dprintf("couldn't create file /usr!\n") ;
        if (!do_mkdir("/usr/true")) dprintf("couldn't create file /usr/true!\n") ;
        if (!do_mkdir("/usr/false/foo")) dprintf("couldn't create file /usr/false/foo!\n") ;
               
}

void test_close()
{
        proc_file pft[NUM_PROC_FILES];
        init_proc_file_table(pft);
        
        do_mkdir("/usr");
        do_mkdir("/usr/mydir");
        do_mkfile("/usr/mydir/mydoc");
        
        file_nr fd = do_open("/usr/mydir/mydoc");       
        file_nr pfd1 = insert_proc_file(pft, fd);
        file_nr pfd2 = insert_proc_file(pft, fd);
        
        dump_files();
        dump_proc_files(pft);
        
        do_close_pf(pft, pfd2);
        dump_files();
        dump_proc_files(pft);
        
        do_close_pf(pft, pfd1);
        dump_files();
        dump_proc_files(pft);
               
}

void test_ls()
{
        dir_entry directory [DIR_ENTRIES_PER_BLOCK];
        bzero(directory, sizeof(directory));
        
        do_create("/usr", DIRECTORY);
        do_create("/usr/file1", DATA_FILE);
        do_create("/usr/file2", DATA_FILE);
        do_create("/usr/file3", DIRECTORY);
        do_create("/usr/file4", DATA_FILE);
        do_create("/usr/file5", DATA_FILE);
        
        file_nr fd = do_open("/usr");
        
        __ls("/usr");
}

void test_sync()
{
        fs_shutdown();
        dump_bmap();
        dump_inode(root);
        load_fs();
        dump_bmap();
        dump_inode(root);
}

void test_delete()
{
        if (!fs_create("/dir1", DIRECTORY)) dprintf("creation unsuccessful!\n");
        if (!fs_create("/dir1/dir2", DIRECTORY)) dprintf("creation unsuccessful!\n");
        
        __ls("/dir1");
        fs_delete("/dir1/dir2");
        __ls("/dir1");
        fs_delete("/dir1");
        __ls("/");
        
}

void run_FS_tests()
{
        //test_rw_quantitative();
}
