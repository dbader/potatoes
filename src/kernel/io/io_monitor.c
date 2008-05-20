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
#include "../include/string.h"
#include "../include/stdlib.h"

static uint16 *disp = (uint16*)0xB8000; //display pointer
static void *scroll_up_ptr;
static void *scroll_down_ptr;
static uint32 num_lines_up = 0;
static uint32 num_lines_down = 0;

void monitor_init()
{
	scroll_up_ptr = malloc_name(10000,"video buffer");
	bzero(scroll_up_ptr,10000);
	scroll_down_ptr = malloc_name(10000,"video buffer");
	bzero(scroll_down_ptr,10000);
}
/**
 * Scrolls the monitor down in the 'natural' way
 */
void monitor_scroll()
{
	num_lines_up++;
	scroll_up_ptr += 0xA0;
	memcpy(scroll_up_ptr, (void*)0xB8000, 0xA0);
	memcpy((void*)0xB8000, (void*)0xB80A0, 0xFA0);
	disp -= 0x50;
}

/**
 * Scrolls the monitor up on request
 */
void monitor_scrollup()
{
	if (num_lines_up > 0) {
		num_lines_up--;
		num_lines_down++;
		scroll_down_ptr += 0xA0;
		memcpy(scroll_down_ptr, (void*)0xB8F00, 0xA0);
		memcpy((void*)0xB80A0, (void*)0xB8000, 0xF00);
		memcpy((void*)0xB8000, scroll_up_ptr, 0xA0);
		scroll_up_ptr -= 0xA0;
	}
}

/**
 * Scrolls the monitor down on request
 */
void monitor_scrolldown()
{
	if (num_lines_down > 0) {
		num_lines_down--;
		num_lines_up++;
		scroll_up_ptr += 0xA0;
		memcpy(scroll_up_ptr, (void*)0xB8000, 0xA0);
		memcpy((void*)0xB8000, (void*)0xB80A0, 0xF00);
		memcpy((void*)0xB8F00, scroll_down_ptr, 0xA0);
		scroll_down_ptr -= 0xA0;
	}
}

/**
 *  writes a colored character to the display (fg=foregroung, bg=background)
 */	
void monitor_cputc(char ch, uint8 fg, uint8 bg)
{
        uint32 i=0;
        uint32 temp;
        if((uint32)disp >= 0xB8FA0)monitor_scroll(); //scroll, if outside of display-memory
        switch(ch){
        case '\n':
                temp= 0x50 - (((uint32)disp - 0xB8000) % 0xA0) / 2;
                while(i < temp){ //calculating the "new line" starting position
                        i++;
                        monitor_cputc(' ',fg,bg);
                }
                break;
        case '\t':
                temp = 0x8 - (((uint32)disp - 0xB8000) % 0x10) / 2;
                while(i < temp){ //calculating the "next tab" starting position
                        i++;
                        monitor_cputc(' ',fg,bg);
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
void monitor_cputs(char *str, uint8 fg, uint8 bg)
{
        while(*str!=0){
                monitor_cputc(*str, fg, bg);
                str += 1;
        }
}

/**
 * writes a character to the display
 */
void monitor_putc(char ch)
{
        monitor_cputc(ch, WHITE, BLACK);
}

/**
 * writes a string to the display
 */
void monitor_puts(char *str)
{
        monitor_cputs(str, WHITE, BLACK);
}

/**
 * writes an integer to the display
 */
void monitor_puti(sint32 x)
{
        int div=1000000000;
        if(x < 0){
                monitor_cputc('-',RED,BLACK);
                x = -x;
        }
        else if (x == 0) {monitor_cputc('0',RED,BLACK); return;}
        while(div > x)
                        div /= 10;
        while(div > 0){
                monitor_cputc((uint8)(x / div + 48),RED,BLACK);
                x%=div;
                div /= 10;
        }    
}

/**
 * writes a hex-byte to the display
 */
void monitor_puthex(uint8 ch)
{
        uint8 low = ch % 16;
        uint8 high = ch / 16;  
        if(high>9)
                monitor_cputc((high + 55),GREEN,BLACK);
        else
                monitor_cputc((high + 48),GREEN,BLACK);   
        if(low>9)
                monitor_cputc((low + 55),GREEN,BLACK);
        else
                monitor_cputc((low + 48),GREEN,BLACK);
}
