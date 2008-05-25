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
 * @author $LastChangedBy$
 * @version $Rev$
 *
 */
 
#include "../include/types.h"
#include "../include/stdio.h"
#include "../include/string.h"
#include "../include/stdlib.h"
#include "../include/util.h"
#include "../include/ringbuffer.h"

#include "../io/io.h"

static uint16 *disp = (uint16*)0xB8000; //display pointer
ring_fifo *scroll_up_ptr;
ring_fifo *scroll_down_ptr;
static uint32 num_lines_up = 0;
static uint32 num_lines_down = 0;
static uint16 charnum = 0;

//FIXME: prevent scrolling writing everywhere in the memory (set limits)!!!
void monitor_init()
{
	//160KB = 1000 * 160B = 1000 lines
	scroll_up_ptr = rf_alloc(160000);
	scroll_down_ptr = rf_alloc(160000);
}
/**
 * Scrolls the monitor down in the 'natural' way
 */
void monitor_scroll()
{
	num_lines_up++;
	rf_write(scroll_up_ptr,(uint8*)0xB8000, 0xA0);
	memmove((void*)0xB8000, (void*)0xB80A0, 0xF00);
	for(uint16 *temp = (uint16*)0xB8F00; temp < (uint16*)0xB8FA0; temp++){
		*temp = 0xF00;
	}
	disp -= 0x50;
	charnum -= 80;
	
	//Cursor update
	outb(0x3D4, 0x0E);
	outb(0x3D5, charnum >> 8);
	outb(0x3D4, 0x0F);
	outb(0x3D5, charnum);
}

/**
 * Scrolls the monitor up on request
 */
void monitor_scrollup()
{
	if (num_lines_up > 0) {
		num_lines_up--;
		num_lines_down++;
		rf_write(scroll_down_ptr,(uint8*)0xB8F00, 0xA0);
		memmove((void*)0xB80A0, (void*)0xB8000, 0xF00);
		rf_read(scroll_up_ptr, (uint8*)0xB8000, 0xA0);
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
		rf_write(scroll_up_ptr,(uint8*)0xB8000, 0xA0);
		memmove((void*)0xB8000, (void*)0xB80A0, 0xF00);
		rf_read(scroll_down_ptr, (uint8*)0xB8F00, 0xA0);
	}
}

/**
 *  writes a colored character to the display (fg=foregroung, bg=background)
 */	
void monitor_cputc(char ch, uint8 fg, uint8 bg)
{
        uint32 i=0;
        uint32 temp;
        if(charnum >= 1999)monitor_scroll(); //scroll, if outside of display-memory
        switch(ch){
        case '\n':
                temp= 0x50 - (((uint32)disp - 0xB8000) % 0xA0) / 2;
                while(i++ < temp){ //calculating the "new line" starting position
                        monitor_cputc(' ',fg,bg);
                }
                break;
        case '\t':
                temp = 0x8 - (((uint32)disp - 0xB8000) % 0x10) / 2;
                while(i++ < temp){ //calculating the "next tab" starting position
                        monitor_cputc(' ',fg,bg);
                }
                break;
        case '\b':
                disp--;
                charnum--;
                *disp = bg * 0x1000 + fg * 0x100 + ' ';
                break;
        default:
                *disp = bg * 0x1000 + fg * 0x100 + ch; //print character to the display pointer
                disp++;
                charnum++;
        }
        outb(0x3D4, 0x0E);
        outb(0x3D5, charnum >> 8);
        outb(0x3D4, 0x0F);
        outb(0x3D5, charnum);
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
        monitor_putc('\n');
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

void monitor_invert()
{
	for(uint8 *temp = (uint8*)0xB8001; (uint32)temp < 0xB8FA0; temp+=2)
		*temp = 0xFF - *temp;
}
