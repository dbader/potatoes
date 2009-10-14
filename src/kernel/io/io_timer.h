/*$Id: io_timer.h 244 2008-12-16 18:47:14Z dtraytel $
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
