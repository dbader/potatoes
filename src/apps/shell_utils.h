/* $Id: shell_utils.h 266 2009-10-14 08:15:36Z dtraytel $
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
 * Shell utility functions.
 *
 * @author Daniel Bader
 * @author $LastChangedBy: dtraytel $
 * @version $Rev: 266 $
 */

#ifndef __SHELL_UTILS_H
#define __SHELL_UTILS_H

extern int STDIN;
extern int STDOUT;

int _fputch(char ch, int fd);
int _fgetch(int fd);
char* _fgets(char *s, int n, int fd);
int _fputs(char *s, int fd);
void _printf(char *fmt, ...);
char* shell_makepath(char *path);

#endif /* __SHELL_UTILS_H */
