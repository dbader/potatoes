/* $Id: syscalls_cli.h 266 2009-10-14 08:15:36Z dtraytel $
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
 * Header file for the "client" side syscall magic.
 *
 * @author dbader
 * @author $LastChangedBy: dtraytel $
 * @version $Rev: 266 $
 */

#ifndef __SYSCALLS_CLI_H
#define __SYSCALLS_CLI_H

#include "../include/types.h"
#include "../fs/fs_types.h"
#include "syscalls_shared.h"

/*FIXME: These will be refactored to lose the leading underscore as soon
 *       as we have external programs. Right now I want to minimize
 *       naming conflicts.
 */
void _log(char* msg);
void _exit(int status);
int _getpid();
int _open(char *path, int oflag, int mode);
int _close(int fd);
int _read(int fd, void *buf, int size);
int _write(int fd, void *buf, int size);
int _seek(int fd, int offset, int whence);
void* _malloc(size_t size);
void _free(void *block);
int _unlink(char* path);
int _stat(char *path, stat *buf);
void _kill(int pid);

void _printf(char *fmt, ...);
char* _fgets(char *s, int n, int fd);
int _fputs(char *s, int fd);
int _fgetch(int fd);
int _fputch(char ch, int fd);

#endif /* syscalls_cli.h */
