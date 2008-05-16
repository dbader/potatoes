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
 * Definition of the ASSERT() macro.
 *
 * @author Daniel Bader
 * @author $LastChangedBy$
 * @version $Rev$
 */
#ifndef __ASSERT_H
#define __ASSERT_H

#define ASSERT(x) if (!(x)) { \
        printf("ASSERTION FAILED: %s:%s():%d. Expr: %s\n", __FILE__, __FUNCTION__, __LINE__, #x); }

#endif /* assert.h */
