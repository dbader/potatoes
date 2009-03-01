/*$Id: io_timer.h 244 2008-12-16 18:47:14Z dtraytel $
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
 * header file for the timer-handler
 *
 * @author Dmitriy Traytel
 * @author $LastChangedBy: dtraytel $
 * @version $Rev: 244 $
 */

#ifndef __IO_TIMER_H_
#define __IO_TIMER_H_

#define PIT_COUNTER0 0x40
#define PIT_COUNTER1 0x41
#define PIT_COUNTER2 0x42
#define PIT_CONTROL 0x43

/* 0x36=00.11.011.0b
 * ---------------------------------------
 * 00-Select counter 0
 * 11-Read/load LSB first then MSB of the counter
 * 011-Mode 3: Square wave rate Generator
 * 0-Binary counter
 */
#define PIT_INIT_CMD 0x36

/* 0xB6=10.11.011.0b
 * 10-Select counter 2
 * 11-Read/load LSB first then MSB of the counter
 * 011-Mode 3: Square wave rate Generator
 * 0-Binary counter
 */
#define PIT_SOUND_CMD 0xB6

#endif /* __IO_TIMER_H_ */
