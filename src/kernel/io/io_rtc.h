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
 * The real-time clock constants and the time struct
 *
 * @author Dmitriy Traytel
 * @author $LastChangedBy$
 * @version $Rev$
 */

#ifndef __IO_RTC_H
#define __IO_RTC_H

#include "../include/types.h"

#define RTC_ADDR 0x70
#define RTC_DATA 0x71

#define RTC_SEC 0
#define RTC_ALERTSEC 1
#define RTC_MIN 2
#define RTC_ALERTMIN 3
#define RTC_HOUR 4
#define RTC_ALERTHOUR 5
#define RTC_WEEKDAY 6
#define RTC_DAY 7
#define RTC_MONTH 8
#define RTC_YEAR 9
#define RTC_STATA 10
#define RTC_STATB 11
#define RTC_STATC 12
#define RTC_STATD 13
#define RTC_CENTURY 50
/**
 * Global time struct.
 */
struct time {
        uint8 sec;
        uint8 min;
        uint8 hour;
        uint8 weekday;
        uint8 day;
        uint8 month;
        uint8 year;
        uint8 century;
}time;

void rtc_init();
void rtc_update();
char* time2str(time_t timestamp, char buf[24]);

#endif /*__IO_RTC_H*/
