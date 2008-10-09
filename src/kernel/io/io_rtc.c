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
 * The real-time clock
 *
 * @author Dmitriy Traytel
 * @author $LastChangedBy$
 * @version $Rev$
 */

#include "../io/io_rtc.h"
#include "../io/io.h"

#include "../include/init.h"
#include "../include/stdio.h"
#include "../include/stdlib.h"
#include "../include/string.h"
#include "../include/types.h"

/**
 * Initializes the global data struct
 */
void rtc_init()
{
        // TODO: is this necessary? 
        // sets rtc parameters
        // outb(RTC_ADDR, RTC_STATB);
        // outb(RTC_DATA, 0x7E);

        outb(RTC_ADDR, RTC_SEC);
        time.sec = inb(RTC_DATA);
        outb(RTC_ADDR, RTC_MIN);
        time.min = inb(RTC_DATA);
        outb(RTC_ADDR, RTC_HOUR);
        time.hour = inb(RTC_DATA);
        outb(RTC_ADDR, RTC_DAY);
        time.day = inb(RTC_DATA);
        outb(RTC_ADDR, RTC_WEEKDAY);
        time.weekday = inb(RTC_DATA);
        
// FIXME: this is dirty: QEMU's rtc encodes the weekday differently from other rtcs
#ifdef QEMU
        time.weekday++;
#endif
        outb(RTC_ADDR, RTC_MONTH);
        time.month = inb(RTC_DATA);
        outb(RTC_ADDR, RTC_YEAR);
        time.year = inb(RTC_DATA);
}

/**
 * Updates the date-data in the global time struct
 */
void rtc_update()
{
        outb(RTC_ADDR, RTC_SEC);
        time.sec = inb(RTC_DATA);
        if (time.sec == 0) {
                outb(RTC_ADDR, RTC_MIN);
                time.min = inb(RTC_DATA);
                if (time.min == 0) {
                        outb(RTC_ADDR, RTC_HOUR);
                        time.hour = inb(RTC_DATA);
                        if (time.hour == 0) {
                                outb(RTC_ADDR, RTC_DAY);
                                time.day = inb(RTC_DATA);
                                outb(RTC_ADDR, RTC_WEEKDAY);
                                time.weekday = inb(RTC_DATA);
#ifdef QEMU
                                time.weekday++;
#endif
                                if (time.day == 1) {
                                        outb(RTC_ADDR, RTC_MONTH);
                                        time.month = inb(RTC_DATA);
                                        if (time.sec == 1) {
                                                outb(RTC_ADDR, RTC_YEAR);
                                                time.year = inb(RTC_DATA);
                                        }
                                }
                        }
                }
        }
}

/**
 * Converts the data in the global time struct into a readable string
 * 
 * @param timestamp the buffer for the date-string
 * @return the pointer to the same buffer now filled with the date string
 */
char* time2str(char timestamp[24])
{
        switch (time.weekday) {
        case 1:
                strcpy(timestamp, "SUN");
                break;
        case 2:
                strcpy(timestamp, "MON");
                break;
        case 3:
                strcpy(timestamp, "TUE");
                break;
        case 4:
                strcpy(timestamp, "WEN");
                break;
        case 5:
                strcpy(timestamp, "THU");
                break;
        case 6:
                strcpy(timestamp, "FRI");
                break;
        case 7:
                strcpy(timestamp, "SAT");
                break;
        default:
                return "Date error              ";
        }
        timestamp[3] = ' ';
        if (time.day < 10) {
                timestamp[4] = '0';
                itoa(time.day, timestamp + 5, 16);
        } else {
                itoa(time.day, timestamp + 4, 16);
        }
        timestamp[6] = '.';
        if (time.month < 10) {
                timestamp[7] = '0';
                itoa(time.month, timestamp + 8, 16);
        } else {
                itoa(time.month, timestamp + 7, 16);
        }
        timestamp[9] = '.';
        if (time.year < 10) {
                strcpy(timestamp + 10, "200");
                itoa(time.year, timestamp + 13, 16);
        } else if (time.year < 100) {
                strcpy(timestamp + 10, "20");
                itoa(time.month, timestamp + 12, 16);
        } else {
                strcpy(timestamp + 10, "2");
                itoa(time.month, timestamp + 11, 16);
        }
        timestamp[14] = ' ';
        if (time.hour < 10) {
                timestamp[15] = '0';
                itoa(time.hour, timestamp + 16, 16);
        } else {
                itoa(time.hour, timestamp + 15, 16);
        }
        timestamp[17] = ':';
        if (time.min < 10) {
                timestamp[18] = '0';
                itoa(time.min, timestamp + 19, 16);
        } else {
                itoa(time.min, timestamp + 18, 16);
        }
        timestamp[20] = ':';
        if (time.sec < 10) {
                timestamp[21] = '0';
                itoa(time.sec, timestamp + 22, 16);
        } else {
                itoa(time.sec, timestamp + 21, 16);
        }
        timestamp[23] = 0;
        return timestamp;
}
