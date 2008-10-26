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
 * Basic definitions of all functions concerning work on files
 *
 * @author Vincenz Doelle
 * @author $LastChangedBy$
 * @version $Rev$
 */

#ifndef __FS_MAIN_H_
#define __FS_MAIN_H_


/* MKFS */
void fs_init();
void fs_shutdown();

bool load_fs();
bool create_fs();


/* CREATE */
bool do_create(char *abs_path, uint8 mode);
bool do_mkdir(char *abs_path);
bool do_mkfile(char *abs_path);

/* DELETE */
int do_remove(char *abs_path);

/* OPEN */
file_nr do_open(char *abs_path);

/* CLOSE */
int do_close(file_nr fd);
int do_close_pf(proc_file pft[NUM_PROC_FILES], file_nr pfd);

/* READ */
size_t do_read(file_nr fd, void *buf, size_t count, uint32 pos);

/* WRITE */
size_t do_write(file_nr fd, void *buf, size_t count, uint32 pos);

/* LSEEK */
size_t do_lseek(proc_file pft[NUM_PROC_FILES], file_nr fd, sint32 offset, uint32 origin);

/* FILE INFO */
file_info_t* get_file_info(file_nr fd, file_info_t* info);

/* FILE EXISTS */
bool do_file_exists(char *path);

/* DEBUG */
void dump_consts();

#endif /*__FS_MAIN_*/
