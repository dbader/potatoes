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
char* time2str(time_t timestamp, char buf[24])
{
        switch (timestamp.weekday) {
        case 1:
                strcpy(buf, "SUN");
                break;
        case 2:
                strcpy(buf, "MON");
                break;
        case 3:
                strcpy(buf, "TUE");
                break;
        case 4:
                strcpy(buf, "WEN");
                break;
        case 5:
                strcpy(buf, "THU");
                break;
        case 6:
                strcpy(buf, "FRI");
                break;
        case 7:
                strcpy(buf, "SAT");
                break;
        default:
                return "Date error              ";
        }
        buf[3] = ' ';
        if (timestamp.day < 10) {
                buf[4] = '0';
                itoa(timestamp.day, buf + 5, 16);
        } else {
                itoa(timestamp.day, buf + 4, 16);
        }
        buf[6] = '.';
        if (timestamp.month < 10) {
                buf[7] = '0';
                itoa(timestamp.month, buf + 8, 16);
        } else {
                itoa(timestamp.month, buf + 7, 16);
        }
        buf[9] = '.';
        if (timestamp.year < 10) {
                strcpy(buf + 10, "200");
                itoa(timestamp.year, buf + 13, 16);
        } else if (timestamp.year < 100) {
                strcpy(buf + 10, "20");
                itoa(timestamp.month, buf + 12, 16);
        } else {
                strcpy(buf + 10, "2");
                itoa(timestamp.month, buf + 11, 16);
        }
        buf[14] = ' ';
        if (timestamp.hour < 10) {
                buf[15] = '0';
                itoa(timestamp.hour, buf + 16, 16);
        } else {
                itoa(timestamp.hour, buf + 15, 16);
        }
        buf[17] = ':';
        if (timestamp.min < 10) {
                buf[18] = '0';
                itoa(timestamp.min, buf + 19, 16);
        } else {
                itoa(timestamp.min, buf + 18, 16);
        }
        buf[20] = ':';
        if (timestamp.sec < 10) {
                buf[21] = '0';
                itoa(timestamp.sec, buf + 22, 16);
        } else {
                itoa(timestamp.sec, buf + 21, 16);
        }
        buf[23] = 0;
        return buf;
}
