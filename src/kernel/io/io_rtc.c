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
 * Converts a bcd-value into a binary value
 *
 * @param num the bcd-value to convert
 * @return the corresponding binary value
 */
uint8 bcd2bin(uint8 num)
{
        return (num / 16) * 10 + (num % 16);
}

/**
 * Calculates the weekday with the algorithm from
 * http://en.wikipedia.org/wiki/Calculating_the_day_of_the_week
 *
 * @param ts the pointer to the time struct which weekday shall be calculated
 */
void calculate_weekday(time_t *ts)
{
        int month[] = {0,3,3,6,1,4,6,2,5,0,3,5};
        int year = bcd2bin(ts->year) + (bcd2bin(ts->year) / 4);
        int century = 2 * (3 - (20 % 4));

        //January/February of a leap year
        int correction = (bcd2bin(ts->year)%4==0 &&
                        (bcd2bin(ts->year)%100!=0 || bcd2bin(ts->year)%400==0))&&
                        (bcd2bin(ts->month)<3)?6:0;

        ts->weekday = (century +
                        year +
                        month[bcd2bin(ts->month) - 1] +
                        bcd2bin(ts->day) +
                        correction
        ) % 7;
}

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
        outb(RTC_ADDR, RTC_MONTH);
        time.month = inb(RTC_DATA);
        outb(RTC_ADDR, RTC_YEAR);
        time.year = inb(RTC_DATA);
        outb(RTC_ADDR, RTC_CENTURY);
        time.century = inb(RTC_DATA);

        calculate_weekday(&time);
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
                                if (time.day == 1) {
                                        outb(RTC_ADDR, RTC_MONTH);
                                        time.month = inb(RTC_DATA);
                                        if (time.month == 1) {
                                                outb(RTC_ADDR, RTC_YEAR);
                                                time.year = inb(RTC_DATA);
                                                if (time.year == 1) {
                                                        outb(RTC_ADDR, RTC_CENTURY);
                                                        time.century = inb(RTC_DATA);
                                                }
                                        }
                                }
                                calculate_weekday(&time);
                        }
                }
        }
}

/**
 * Converts the data in the global time struct into a readable string
 *
 * @param timestamp the actual time struct to be converted
 * @param buf the buffer for the date-string
 * @return the pointer to the same buffer now filled with the date string
 */
char* time2str(time_t timestamp, char buf[24])
{
        switch (timestamp.weekday) {
        case 0:
                strcpy(buf, "SUN");
                break;
        case 1:
                strcpy(buf, "MON");
                break;
        case 2:
                strcpy(buf, "TUE");
                break;
        case 3:
                strcpy(buf, "WEN");
                break;
        case 4:
                strcpy(buf, "THU");
                break;
        case 5:
                strcpy(buf, "FRI");
                break;
        case 6:
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
        if (timestamp.century < 10) {
                strcpy(buf + 10, "0");
                itoa(timestamp.century, buf + 11, 16);
        } else {
                itoa(timestamp.century, buf + 10, 16);
        }
        if (timestamp.year < 10) {
                strcpy(buf + 12, "0");
                itoa(timestamp.year, buf + 13, 16);
        } else {
                itoa(timestamp.year, buf + 12, 16);
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
