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

#include "../include/const.h"
#include "../include/debug.h"
#include "../io/io.h"
#include "../include/ringbuffer.h"
#include "pm_main.h"

/**
 * Handles keyboard input. This gets called from the I/O code after a keypress was received
 * and translated. pm_handle_input() then distributes incoming character to the focussed
 * process' input queue.
 * 
 * @param c input character
 */
void pm_handle_input(char c)
{
        if (focus_proc == NULL) {
                return;
        }
        
        //TODO: This is the PERFORMANCE FIX
        if (focus_proc->state == PSTATE_STDINSLEEP) {
                focus_proc->state = PSTATE_ALIVE;
        }
        
        if (rf_write(focus_proc->stdin, &c, sizeof(c)) != sizeof(c)) {
                dprintf("pm_handle_input: error writing to stdin\n");
        }
}

