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
#include "../include/types.h"
#include "../include/stdio.h"  

static sint32 ticks = 0;

void timer_handler(){
        ticks++;
        //call pm
        if(ticks == MAX_SINT32) ticks = 0;
        //printf("%d\n",ticks);
}

/**
 * sleeps sec seconds
 */
void sleep(sint32 sec){
        sint32 i = ticks + FREQUENCY*sec;
        if(MAX_SINT32 - ticks < FREQUENCY*sec){
                sleep((MAX_SINT32-ticks)/FREQUENCY);
                sleep(sec-(MAX_SINT32-ticks)/FREQUENCY); 
        }
        else
        {
                while(ticks < i){}
        }
}

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
