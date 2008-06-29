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

void* malloc(uint32 size);
void* mallocn(uint32 size, char *name);
void* calloc(size_t elements, size_t size);
void* callocn(size_t elements, size_t size, char* name);
void free(void *start);
void* realloc(void *pointer, size_t size);
uint32 free_memory();

#endif /* stdlib.h */
