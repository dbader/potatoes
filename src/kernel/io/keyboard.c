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
 * Keyboard-handler
 * 
 * @author Dmitriy Traytel
 * @author $LastChangedBy$
 * @version $Rev$
 */
 
#include "../include/stdio.h" 
#include "../include/types.h"

#include "../io/keyboard.h"

void kb_handler()
{
        uint8 scancode = inb(0x60);
        if(scancode & 0x80){
                if((scancode & (~0x80)) == LSHIFT || (scancode & (~0x80)) == RSHIFT){
                        shift = 0;
                }
                else if((scancode & (~0x80)) == ALT)
                        alt = 0;
        }   
        else if(shift){
                if (kb_shift_map[scancode] != 0)
                        putc(kb_shift_map[scancode]);
        }
        else if(alt){
                if (kb_alt_map[scancode] != 0)
                        putc(kb_alt_map[scancode]);
        }     
        else{
                if(scancode == LSHIFT || scancode == RSHIFT)
                        shift = 1;
                else if(scancode == ALT)
                        alt = 1;
                else if (kb_map[scancode] != 0)
                        putc(kb_map[scancode]);
        }
}
