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
 * Timer-handler
 *
 * @author Dmitriy Traytel
 * @author $LastChangedBy$
 * @version $Rev$
 */

#include "../include/const.h"
#include "../include/limits.h"
#include "../include/types.h"
#include "../include/stdio.h"

#include "../io/io.h"
#include "../io/io_timer.h"
#include "../io/io_rtc.h"
#include "../io/io_virtual.h"
#include "../pm/pm_main.h"

/**
 * This variable is incremented after every IRQ0 (or reseted to 0)
 */
static sint32 ticks = 0;

/**
 * Handles a timer-interrupt by incrementing the ticks-counter and calling the PM.
 */
uint32 timer_handler(uint32 context)
{
        ticks++;
        if (ticks == SINT32_MAX)
                ticks = 0;

        rtc_update();
        update_virt_monitor(get_active_virt_monitor());

        return pm_schedule(context);
}

/**
 * Returns the timer ticks.
 * @return ticks since system boot or last timer wraparound
 */
sint32 get_ticks()
{
        return ticks;
}

/**
 * Sleeps num seconds
 */
void sleep(sint32 num)
{
        sint32 i = ticks + FREQUENCY * num;
        if (SINT32_MAX - ticks < FREQUENCY*num) {
                sleep((SINT32_MAX - ticks) / FREQUENCY);
                sleep(num - (SINT32_MAX - ticks) / FREQUENCY);
        } else {
                reactivate_pic();
                set_interrupts();
                while (ticks < i) {
                        halt();
                }
        }
}

/**
 * Sleeps num ticks
 */
void sleep_ticks(sint32 num)
{
        sint32 i = ticks + num;
        if (SINT32_MAX - ticks < num) {
                sleep(SINT32_MAX - ticks);
                sleep(num - (SINT32_MAX - ticks));
        } else {
                reactivate_pic();
                set_interrupts();
                while (ticks < i) {
                        halt();
                }
        }
}

/**
 * Initializes the timer with the given frequency freq
 */
void timer_init(sint32 freq)
{
        int counter = 1193180 / freq; //1193180Hz - base 8254 frequency
        outb(PIT_CONTROL, PIT_INIT_CMD);

        outb(PIT_COUNTER0, counter % 0xFF); //LSB
        outb(PIT_COUNTER0, counter / 0xFF); //MSB
}
