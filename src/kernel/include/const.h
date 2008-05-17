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
 * Basic constant definitions. This file defines constants used
 * throughout the kernel.
 *
 * @author Daniel Bader
 * @author $LastChangedBy$
 * @version $Rev$
 */

#ifndef __CONST_H
#define __CONST_H

#define NULL            0
#define TRUE            1
#define FALSE           0
#define MAX_SINT32      2147483647
#define FREQUENCY       200 //inits timer with frequency 200Hz (shoots every 5 ms)

#endif /* const.h */
