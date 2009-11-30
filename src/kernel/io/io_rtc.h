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

uint8 bcd2bin(uint8 num);
void rtc_init();
void rtc_update();
char* time2str(time_t timestamp, char buf[24]);

#endif /*__IO_RTC_H*/
