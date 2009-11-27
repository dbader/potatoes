/* $Id: limits.h 266 2009-10-14 08:15:36Z dtraytel $
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
 * Definition of the maximum and minimum values of different types.
 *
 * @author Dmitriy Traytel
 * @author $LastChangedBy: dtraytel $
 * @version $Rev: 266 $
 */

#ifndef __LIMITS_H
#define __LIMITS_H

#define SINT8_MAX       0x7F
#define UINT8_MAX       0xFF
#define SINT16_MAX      0x7FFF
#define UINT16_MAX      0xFFFF
#define SINT32_MAX      0x7FFFFFFF
#define UINT32_MAX      0xFFFFFFFF

#define SINT8_MIN       0x80
#define SINT16_MIN      0x8000
#define SINT32_MIN      0x80000000

#endif /* limits.h */
