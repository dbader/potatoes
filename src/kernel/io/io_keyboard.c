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

#include "../include/const.h" 
#include "../include/types.h"
#include "../include/stdio.h"
#include "../include/stdlib.h"
#include "../include/assert.h"

#include "../io/io.h"
#include "../io/io_keyboard.h"
#include "../pm/pm_input.h"

/**
 * Echo-mode flag
 */
bool echo = TRUE; //accessible for PM

/**
 * Number of registered shortcuts
 */
uint8 shcut_num = 0;

/**
 * the keyboard shortcuts struct
 */
struct shortcut shortcuts[100];

/**
 * Adds a shortcut to the system
 * 
 * @param control_flag          is ctrl the initiator?
 * @param super_flag            is super the initiator?
 * @param character             shortcut character
 * @param (*function)()         function to be called 
 */
void add_shortcut(bool control_flag, bool super_flag, uint8 character, void (*function)())
{
        shortcuts[shcut_num].ch = character;
        shortcuts[shcut_num].control = control_flag;
        shortcuts[shcut_num].super = super_flag;
        shortcuts[shcut_num].func = function;
        if (shcut_num < 99) shcut_num++;
}

/**
 * Handles an keyboard interrupt, by calling the PM (providing already the right character). 
 * In echo-mode prints the typed character directly to the screen.
 */
void kb_handler()
{
        uint8 scancode = inb(0x60);
        if (scancode & 0x80) { //Key released
                if ((scancode & (~0x80)) == LSHIFT || (scancode & (~0x80)) == RSHIFT)
                        shift = 0;
                else if ((scancode & (~0x80)) == ALT)
                        alt = 0;
                else if ((scancode & (~0x80)) == CTRL)
                        ctrl = 0;
                else if ((scancode & (~0x80)) == SUPER)
                        super_button = 0;
        }   
        else if (shift) { //Key pressed while shift is pressed
                if (kb_shift_map[scancode] != 0) {
                        if(echo) monitor_putc(kb_shift_map[scancode]);
                        else pm_handle_input(kb_map[scancode]);
                }
        }
        else if (alt) { //Key pressed while alt is pressed
                if (scancode == 0x12) monitor_invert();
                if (kb_alt_map[scancode] != 0) {
                        if(echo) monitor_putc(kb_alt_map[scancode]);
                        else pm_handle_input(kb_map[scancode]);
                }
        }
        else if (ctrl && super_button) { //Key pressed while ctrl & super are pressed
                int i = 0;
                for(i; i<=shcut_num; i++)
                        if ( shortcuts[i].control && shortcuts[i].super  
                                        && shortcuts[i].ch == kb_map[scancode] ){
                                shortcuts[i].func();
                                break;
                        }               
        }
        else if (ctrl && scancode != SUPER) { //Key pressed while ctrl is pressed
                int i = 0;
                for(i; i<=shcut_num; i++)
                        if ( shortcuts[i].control && !shortcuts[i].super
                                        && shortcuts[i].ch == kb_map[scancode] ){
                                shortcuts[i].func();
                                break;
                        }
        }
        else if (super_button && scancode != CTRL) { //Key pressed while super is pressed
                int i = 0;
                for(i; i<=shcut_num; i++)
                        if ( shortcuts[i].super && !shortcuts[i].control
                                        && shortcuts[i].ch == kb_map[scancode] ){
                                shortcuts[i].func();
                                break;
                        }
        }
        else{ //Key pressed
                switch (scancode)
                {
                case CTRL:
                        ctrl = 1;
                        break;
                case SUPER:
                        super_button = 1;
                        break;
                case CURSOR_UP:
                        cursor_move(0);
                        break;
                case CURSOR_DOWN:
                        cursor_move(1);
                        break;
                case CURSOR_LEFT:
                        cursor_move(2);
                        break;
                case CURSOR_RIGHT:
                        cursor_move(3);
                        break;
                case LSHIFT || RSHIFT:
                shift = 1;
                break;
                case ALT:
                        alt = 1;
                        break;
                case SCROLL_UP:
                        monitor_scrollup();
                        break;
                case SCROLL_DOWN:
                        monitor_scrolldown();
                        break;
                default:
                        if (kb_map[scancode] != 0)
                                if(echo) monitor_putc(kb_map[scancode]);
                                else pm_handle_input(kb_map[scancode]);
                }
        }
}
