/* $Id: filp.h 37 2008-05-02 22:59:03Z vdoelle $
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
 * The file descriptor table.
 *
 * @author Vincenz Doelle
 * @author $LastChangedBy: vdoelle $
 * @version $Rev: 37 $
 */

#ifndef FILP_H_
#define FILP_H_

/* central struct */

extern file filp[NUM_FILES];


/* definition */

#define NIL_FILE (file *) 0                     /* there is no free filp entry left */

#endif /*FILP_H_*/
