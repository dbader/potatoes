/* $Id: io_sound.c 239 2008-10-26 23:19:15Z dtraytel $
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
 * Sound driver
 *
 * @author Dmitriy Traytel
 * @author $LastChangedBy: dtraytel $
 * @version $Rev: 239 $
 */
#include "../include/types.h"
#include "io_sound.h"
#include "io_timer.h"
#include "io.h"

void start_beep(uint32 freq){
        uint16 note = (uint16)(1193180 / freq);
        outb(PIT_CONTROL, PIT_SOUND_CMD);
        outb(PIT_COUNTER2, note % 0xFF); //LSB
        outb(PIT_COUNTER2, note / 0xFF); //MSB

        uint8 tmp = inb(IO_SPEAKER_PORT);
        if (tmp != (tmp | 3)) {
                outb(IO_SPEAKER_PORT, tmp | 3);
        }
}

void end_beep() {
        outb(IO_SPEAKER_PORT, inb(IO_SPEAKER_PORT) & 0xFC);
}
