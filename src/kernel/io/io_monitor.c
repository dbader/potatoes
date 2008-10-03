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

/**
 * @file
 * Functions to print things on the monitor
 *
 * @author Dmitriy Traytel
 * @author $LastChangedBy$
 * @version $Rev$
 *
 */

#include "../include/const.h"
#include "../include/types.h"
#include "../include/stdio.h"
#include "../include/string.h"
#include "../include/stdlib.h"
#include "../include/util.h"
#include "../include/ringbuffer.h"
#include "../include/assert.h"

#include "../io/io.h"

static uint16 *disp = (uint16*)0xB8000; //display pointer

void set_disp(uint32 addr)
{
        disp = (uint16*)addr;
}
//void *up_buffer_start;
//void *down_buffer_start;
//static uint32 num_lines_up = 0;
//static uint32 num_lines_down = 0;
//static uint16 charnum = 0;
//static uint32 up_offset = 0;
//static uint32 down_offset = 0;
//
////160KB = 1000 * 160B = 1000 lines
//const uint32 io_bufsize = 160000;

///*
// * Moves the cursor in the given direction
// *
// * @param dir 0=UP, 1=DOWN, 2=LEFT, 3=RIGHT
// */
//void cursor_move(uint8 dir)
//{
//        switch (dir){
//        case 0:
//                charnum = (charnum + 2000 - 80) % 2000;
//                break;
//        case 1:
//                charnum = (charnum + 80) % 2000;
//                break;
//        case 2:
//                charnum = (charnum - charnum % 80) + (charnum % 80 +80 - 1) % 80;
//                break;
//        case 3:
//                charnum = (charnum - charnum % 80) + (charnum +1) % 80;
//                break;
//        }
//        disp = (uint16*)0xB8000 + charnum;
//        //Cursor update
//        outb(0x3D4, 0x0E);
//        outb(0x3D5, charnum >> 8);
//        outb(0x3D4, 0x0F);
//        outb(0x3D5, charnum);
//}
//
///**
// * Initializes the monitor buffers.
// */
//void monitor_init()
//{
////        up_buffer_start = mallocn(io_bufsize, "scrolling buffer(up)");
////        ASSERT(up_buffer_start != NULL);
////        bzero(up_buffer_start, io_bufsize);
////        down_buffer_start = mallocn(io_bufsize, "scrolling buffer(down)");
////        ASSERT(down_buffer_start != NULL);
////        bzero(down_buffer_start, io_bufsize);
//
//        //disables the blinking mode
//        inb(0x3DA);
//        outb(0x3C0,0x10);
//        uint8 temp = inb(0x3C1);
//        outb(0x3C1,0x20);
//        inb(0x3DA);
//        outb(0x3C0,0x10);
//        outb(0x3C0,temp & 0xF7);
//        outb(0x3C0,0x20);
//        inb(0x3DA);
//}
//
///**
// * Scrolls the monitor down in the 'natural' way.
// */
//void monitor_scroll()
//{
//        if (num_lines_up < io_bufsize / 0xA0) num_lines_up++;
//        memcpy(up_buffer_start + up_offset,(void*)0xB8000, 0xA0);
//        up_offset = (uint32)(up_offset + 0xA0) % io_bufsize;
//        memmove((void*)0xB8000, (void*)0xB80A0, 0xF00);
//        for(uint16 *temp = (uint16*)0xB8F00; temp < (uint16*)0xB8FA0; temp++){
//                *temp = 0xF00;
//        }
//        disp -= 0x50;
//        charnum -= 80;
//
//        //Cursor update
//        outb(0x3D4, 0x0E);
//        outb(0x3D5, charnum >> 8);
//        outb(0x3D4, 0x0F);
//        outb(0x3D5, charnum);
//}
//
///**
// * Scrolls the monitor up on request.
// */
//void monitor_scrollup()
//{
//        if (num_lines_up > 0) {
//                num_lines_up--;
//                if (num_lines_down < io_bufsize / 0xA0) num_lines_down++;
//                memcpy(down_buffer_start + down_offset, (void*)0xB8F00, 0xA0);
//                down_offset = (uint32)(down_offset + 0xA0) % io_bufsize;
//                memmove((void*)0xB80A0, (void*)0xB8000, 0xF00);
//                up_offset = (uint32)(up_offset + io_bufsize - 0xA0) % io_bufsize;
//                memcpy((void*)0xB8000, up_buffer_start + up_offset, 0xA0);
//        }
//}
//
///**
// * Scrolls the monitor down on request.
// */
//void monitor_scrolldown()
//{
//        if (num_lines_down > 0) {
//                num_lines_down--;
//                if (num_lines_up < io_bufsize / 0xA0) num_lines_up++;
//                memcpy(up_buffer_start + up_offset,(void*)0xB8000, 0xA0);
//                up_offset = (uint32)(up_offset + 0xA0) % io_bufsize;
//                memmove((void*)0xB8000, (void*)0xB80A0, 0xF00);
//                down_offset = (uint32)(down_offset + io_bufsize - 0xA0) % io_bufsize;
//                memcpy((void*)0xB8F00, down_buffer_start + down_offset, 0xA0);
//        }
//}

/**
 *  Writes a colored character to the display.
 *
 * @param ch character to be written
 * @param fg foreground-color
 * @param bg background color
 */
void monitor_cputc(char ch, uint8 fg, uint8 bg)
{
        uint32 i = 0;
        uint32 temp;
        //if(charnum >= 1999)monitor_scroll(); //scroll, if outside of display-memory
        switch (ch) {
        case '\a':
                monitor_invert();
                sleep_ticks(15);
                monitor_invert();
                break;
        case '\n':
                temp = 0x50 - (((uint32)disp - 0xB8000) % 0xA0) / 2;
                while (i++ < temp) { //calculating the "new line" starting position
                        monitor_cputc(' ', fg, bg);
                }
                break;
        case '\t':
                temp = 0x8 - (((uint32)disp - 0xB8000) % 0x10) / 2;
                while (i++ < temp) { //calculating the "next tab" starting position
                        monitor_cputc(' ', fg, bg);
                }
                break;
        case '\b':
                disp--;
//                charnum--;
                *disp = bg * 0x1000 + fg * 0x100 + ' ';
                break;
        default:
                *disp = bg * 0x1000 + fg * 0x100 + ch; //print character to the display pointer
                disp++;
//                charnum++;
        }
//        outb(0x3D4, 0x0E);
//        outb(0x3D5, charnum >> 8);
//        outb(0x3D4, 0x0F);
//        outb(0x3D5, charnum);
}

/**
 *  Writes a colored null-terminated string to the display.
 *
 * @param *str pointer to the string
 * @param fg foreground-color
 * @param bg background color
 */
void monitor_cputs(char *str, uint8 fg, uint8 bg)
{
        while (*str != 0) {
                monitor_cputc(*str, fg, bg);
                str += 1;
        }
}

/**
 * Writes a character to the display.
 *
 * @param ch character to be written
 */
void monitor_putc(char ch)
{
        monitor_cputc(ch, WHITE, BLACK);
}

/**
 * Writes a null-terminated string to the display.
 *
 * @param *str pointer to the string
 */
void monitor_puts(char *str)
{
        monitor_cputs(str, WHITE, BLACK);
}

/**
 * Writes an integer to the display.
 *
 * @param x integer to be written
 */
void monitor_puti(sint32 x)
{
        int div = 1000000000;
        if (x < 0) {
                monitor_cputc('-', RED, BLACK);
                x = -x;
        } else if (x == 0) {
                monitor_cputc('0', RED, BLACK);
                return;
        }
        while (div > x)
                div /= 10;
        while (div > 0) {
                monitor_cputc((uint8)(x / div + 48), RED, BLACK);
                x %= div;
                div /= 10;
        }
        monitor_putc('\n');
}

/**
 * Writes a hex-byte to the display.
 *
 * @param ch character to be written
 */
void monitor_puthex(uint8 ch)
{
        uint8 low = ch % 16;
        uint8 high = ch / 16;
        if (high > 9)
                monitor_cputc((high + 55), GREEN, BLACK);
        else
                monitor_cputc((high + 48), GREEN, BLACK);
        if (low > 9)
                monitor_cputc((low + 55), GREEN, BLACK);
        else
                monitor_cputc((low + 48), GREEN, BLACK);
}

/**
 * Inverts the monitor.
 */
void monitor_invert()
{
        for (uint8 *temp = (uint8*)0xB8001; (uint32)temp < 0xB8FA0; temp += 2)
                * temp = 0xFF - *temp;
}
