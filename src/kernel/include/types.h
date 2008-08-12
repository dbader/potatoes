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
 * Basic type definitions. This file defines basic data types used
 * throughout the kernel. You should use these types whenever possible
 * in order to avoid the possible ambiguity of the builtin types.
 *
 * @author Daniel Bader
 * @author $LastChangedBy$
 * @version $Rev$
 */

#ifndef __TYPES_H
#define __TYPES_H

/* Macros */
#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)

/* Basic types */
typedef unsigned char   uint8;
typedef signed   char   sint8;
typedef unsigned short  uint16;
typedef signed   short  sint16;
typedef unsigned int    uint32;
typedef signed   int    sint32;

typedef float   float32;
typedef double  float64;

typedef uint8   bool;

typedef uint32  size_t;
typedef struct time time_t;

#endif /* types.h */
