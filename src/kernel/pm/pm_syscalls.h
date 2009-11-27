/* $Id: pm_syscalls.h 266 2009-10-14 08:15:36Z dtraytel $
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
 * Header file for the kernel side syscall implementations.
 *
 * @author dbader
 * @author $LastChangedBy: dtraytel $
 * @version $Rev: 266 $
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
void sys_stat(void *data);
void sys_kill(void *data);

/** Pointer to void sys_XXX(void *data) function */
typedef void (*syscall_handler)(void *data);

extern syscall_handler syscall_table[];

#endif /* syscalls_kernel.h */
