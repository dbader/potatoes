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
 * Header file for the "client" side syscall magic.
 *
 * @author dbader
 * @author $LastChangedBy$
 * @version $Rev$
 */

#ifndef __SYSCALLS_CLI_H
#define __SYSCALLS_CLI_H

#include "../include/types.h"

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

void _printf(char *fmt, ...);
char* _fgets(char *s, int n, int fd);
int _fputs(char *s, int fd);
int _fgetch(int fd);
int _fputch(char ch, int fd);

#endif /* syscalls_cli.h */
