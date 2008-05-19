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
 * Definition of the maximum and minimum values of different types.
 *
 * @author Dmitriy Traytel
 * @author $LastChangedBy$
 * @version $Rev$
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
