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
 * Debugging utility functions. This is work in progress so far. I plan on implementing
 * a small toolbox of useful debugging functions, eg debug printf, stack crawling etc.
 *
 * @author dbader
 * @author $LastChangedBy$
 * @version $Rev$
 */

#ifndef __DEBUG_H
#define __DEBUG_H

#include "stdio.h"
#include "../io/io.h"

#define dprintf printf
#define dprint_separator() dprintf("%{--------------------------------------------------------------------------------}", 2);
#define fs_dprintf dprintf
//dprintf("%s ln %d%{:} ", __FILE__, __LINE__, RED); dprintf

#define SHORTCUT_CTRL(ch, func) add_shortcut(TRUE, FALSE, ch, func); \
        printf("\tCTRL + %c ==> %s()\n", ch, #func)

#define SHORTCUT_SUPER(ch, func) add_shortcut(FALSE, TRUE, ch, func); \
        printf("\tSUPER + %c ==> %s()\n", ch, #func)

#define SHORTCUT_CTRL_SUPER(ch, func) add_shortcut(TRUE, TRUE, ch, func); \
        printf("\tCTRL + SUPER + %c ==> %s()\n", ch, #func)

#endif /* debug.h */
