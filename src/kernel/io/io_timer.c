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

static sint32 ticks = 0;

/**
 * Handles a timer-interrupt by incrementing the ticks-counter and calling the PM.
 */
void timer_handler(){
        ticks++;
        //TODO: call switch();
        if(ticks == SINT32_MAX) ticks = 0;
        //printf("%d\n",ticks);
}

/**
 * Sleeps num seconds
 */
void sleep(sint32 num){
        sint32 i = ticks + FREQUENCY*num;
        if(SINT32_MAX - ticks < FREQUENCY*num){
                sleep((SINT32_MAX-ticks)/FREQUENCY);
                sleep(num-(SINT32_MAX-ticks)/FREQUENCY); 
        }
        else
        {
                while(ticks < i){}
        }
}

/**
 * Initializes the timer with the given frequency freq
 */
void timer_init(sint32 freq)
{
    int counter = 1193180 / freq; //1193180Hz - base 8254 frequency
    outb(0x43, 0x36);
    /* 0x36=00.11.011.0b
     * ---------------------------------------
     * 00-Select counter 0
     * 11-Read/load LSB first then MSB
     * 011-Mode 3: Square wave rate Generator
     * 0-Binary counter
     */
     
    outb(0x40, counter % 0xFF); //LSB
    outb(0x40, counter / 0xFF); //BSB
}
