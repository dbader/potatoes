/* $Id: string.h 266 2009-10-14 08:15:36Z dtraytel $
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
 * Headers for string.c
 *
 * @author Dmitriy Traytel
 * @author $LastChangedBy: dtraytel $
 * @version $Rev: 266 $
 */

#ifndef __STRING_H
#define __STRING_H

#include "types.h"

uint32 strlen(char* str);
char* strcpy(char *dest, char *src);
char* strncpy(char *dest, char *src, size_t n);
char* strchr(char *str, char ch);
char* strcat(char *s1, char *s2);
char* strncat(char *s1, char *s2, size_t n);
char* strdup(char* str);
char* strsep(char **str_ptr, char *delims);
sint32 strcmp(char *s1, char *s2);

void* memset(void *dest, uint8 value, size_t count);
void bzero(void *dest, uint32 count);
void* memcpy(void *dest, void *src, size_t count);
void* memmove(void *dest, void *src, size_t count);

char* strreverse(char *str);
char* itoa(int n, char *str, unsigned int base);
int atoi(char *str);

#endif /* string.h */
