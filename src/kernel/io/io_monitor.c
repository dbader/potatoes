/*
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

/*
 * date          stat
 * -----------------------------------------------
 * 01.05.08      first just-playing-version (DT)
 * 02.05.08      positioning fixes
 *               put* (it's a regex :) ) functions write only to the display-memory (B8000-B8FA0) (DT)
 * 04.05.08      updated types (from types.h)
 */
 
/**
 * @file 
 * Functions to print things on the monitor
 * 
 * @author Dmitriy Traytel
 * @author $LastChangedBy: dtraytel $
 * @version $Rev: 58 $
 *
 */
 
#include "../include/types.h"
#include "../include/stdio.h"

static uint16 *disp = (uint16*)0xB8000; //display pointer

/**
 *  writes a colored character to the display (fg=foregroung, bg=background)
 */	
void putc_col(char ch, uint8 fg, uint8 bg)
{
        uint32 i=0;
        uint32 temp;
        if((uint32)disp >= 0xB8FA0)disp-=0x7D0; //return to beginning, if outside of display-memory
        switch(ch){
        case '\n':
                temp= 0x50 - (((uint32)disp - 0xB8000) % 0xA0) / 2;
                while(i < temp){ //calculating the "new line" starting position
                        i++;
                        putc_col(' ',fg,bg);
                }
                break;
        case '\t':
                temp = 0x8 - (((uint32)disp - 0xB8000) % 0x10) / 2;
                while(i < temp){ //calculating the "next tab" starting position
                        i++;
                        putc_col(' ',fg,bg);
                }
                break;
        case '\b':
                disp -= 1;
                *disp = bg * 0x1000 + fg * 0x100 + ' ';
                break;
        default:
                *disp = bg * 0x1000 + fg * 0x100 + ch; //print character to the display pointer
                disp += 1;
	}
}

/**
 *  writes colored a string to the display (fg=foregroung, bg=background)
 */
void puts_col(char *str, uint8 fg, uint8 bg)
{
        while(*str!=0){
                putc_col(*str, fg, bg);
                str += 1;
        }
}

/**
 * writes a character to the display
 */
void putc(char ch)
{
        putc_col(ch, WHITE, BLACK);
}

/**
 * writes a string to the display
 */
void puts(char *str)
{
        puts_col(str, WHITE, BLACK);
}

/**
 * writes an integer to the display
 */
void puti(sint32 x)
{
        int div=1000000000;
        if(x < 0){
                putc_col('-',RED,BLACK);
                x = -x;
        }
        else if (x == 0) {putc_col('0',RED,BLACK); return;}
        while(div > x)
                        div /= 10;
        while(div > 0){
                putc_col((uint8)(x / div + 48),RED,BLACK);
                x%=div;
                div /= 10;
        }    
}

/**
 * writes a hex-byte to the display
 */
void puthex(uint8 ch)
{
        uint8 low = ch % 16;
        uint8 high = ch / 16;  
        if(high>9)
                putc_col((high + 55),GREEN,BLACK);
        else
                putc_col((high + 48),GREEN,BLACK);   
        if(low>9)
                putc_col((low + 55),GREEN,BLACK);
        else
                putc_col((low + 48),GREEN,BLACK); 
        putc_col('h',GREEN,BLACK);
}
