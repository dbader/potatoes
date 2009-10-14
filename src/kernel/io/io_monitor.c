/*
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

static uint16 *disp = (uint16*)VGA_DISPLAY; //display pointer

void set_disp(uint32 addr)
{
        disp = (uint16*)addr;
}

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
        switch (ch) {
        case '\a':
                monitor_invert();
                sleep_ticks(15);
                monitor_invert();
                break;
        case '\n':
                temp = 0x50 - (((uint32)disp - VGA_DISPLAY) % 0xA0) / 2;
                while (i++ < temp) { //calculating the "new line" starting position
                        monitor_cputc(' ', fg, bg);
                }
                break;
        case '\t':
                temp = 0x8 - (((uint32)disp - VGA_DISPLAY) % 0x10) / 2;
                while (i++ < temp) { //calculating the "next tab" starting position
                        monitor_cputc(' ', fg, bg);
                }
                break;
        case '\b':
                disp--;
                *disp = bg * 0x1000 + fg * 0x100 + ' ';
                break;
        default:
                *disp = bg * 0x1000 + fg * 0x100 + ch; //print character to the display pointer
                disp++;
        }
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
