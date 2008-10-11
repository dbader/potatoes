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
 * Header file for the kernel side syscall implementations.
 *
 * @author dbader
 * @author $LastChangedBy$
 * @version $Rev$
 */

#ifndef __SYSCALLS_KERNEL_H
#define __SYSCALLS_KERNEL_H

void sys_log(void *data);
void sys_exit(void *data);
void sys_getpid(void *data);
void sys_open(void *data);
void sys_close(void* data);
void sys_read(void* data);
void sys_write(void* data);
void sys_seek(void* data);
void sys_malloc(void *data);
void sys_free(void *data);
void sys_unlink(void *data);

/** Pointer to void sys_XXX(void *data) function */
typedef void (*syscall_handler)(void *data);

extern syscall_handler syscall_table[];

#endif /* syscalls_kernel.h */
