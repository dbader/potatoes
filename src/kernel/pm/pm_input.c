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
 * Process management code that handles keyboard input. After a keypress an interrupt
 * is generated which gets handled by the I/O code. In io_keyboard.c the keyboard scancode
 * gets converted into an ascii character which is then given to pm_handle_input().
 * 
 * pm_handle_input() then writes the new character to the focussed process' stdin queue.
 * The respective process can then choose to read its stdin queue at any time via a call
 * to read(STDIN, &buf, len). 
 *
 * @author dbader
 * @author $LastChangedBy$
 * @version $Rev$
 */

#include "../include/debug.h"

/**
 * Handles keyboard input. This gets called from the I/O code after a keypress was received
 * and translated. pm_handle_input() then distributes incoming character to the focussed
 * process' input queue.
 * @param c input character
 */
void pm_handle_input(char c)
{
        dprintf("pm_handle_input: %c\n", c);
        //putchar(c);
}

