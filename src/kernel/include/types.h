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
