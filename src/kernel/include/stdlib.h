/* $Id$
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
 * includes some general help functions for type
 * conversion, memory allocation, process control
 * and other purposes.
 *
 * @author Johannes Schamburger
 * @author $LastChangedBy$
 * @version $Rev$
 */

#ifndef __STDLIB_H
#define __STDLIB_H

#include "types.h"

void srand(int seed);
int rand();
void* malloc(uint32 size);
void* mallocn(uint32 size, char *name);
void* calloc(size_t elements, size_t size);
void* callocn(size_t elements, size_t size, char* name);
void free(void *start);
void* realloc(void *pointer, size_t size);
void mem_dump();
uint32 free_memory();

#endif /* stdlib.h */
