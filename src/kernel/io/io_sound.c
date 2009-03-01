/* $Id: io_sound.c 239 2008-10-26 23:19:15Z dtraytel $
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
