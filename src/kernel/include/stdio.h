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
 * Standard I/O functions
 *
 * @author Dmitriy Traytel
 * @author $LastChangedBy$
 * @version $Rev$
 *
 */

#ifndef __STDIO_H
#define __STDIO_H

#include "types.h"

enum colors{
        BLACK=0x0,
        BLUE=0x1,
        GREEN=0x2,
        CYAN=0x3,
        RED=0x4,
        VIOLET=0x5,
        ORANGE=0x6,
        LIGHTGREY=0x7,
        DARKGREY=0x8,
        LIGHTBLUE=0x9,
        LIGHTGREEN=0xA,
        TURQUOISE=0xB,
        PINK=0xC,
        MAGENTA=0xD,
        YELLOW=0xE,
        WHITE=0xF,
};

//prints colored character
void monitor_cputc(char ch, uint8 fg, uint8 bg);
//prints colored string
void monitor_cputs(char *str, uint8 fg, uint8 bg);
//prints black character
void monitor_putc(char ch);
//prints black string
void monitor_puts(char *str);
//prints an integer
void monitor_puti(sint32 x);
//prints a hex-byte - important for io-debugging (-for me! ;-))
void monitor_puthex(uint8 ch);

//scrolls the monitor up
void monitor_scrollup();
//scrolls the monitor down
void monitor_scrolldown();

int putchar(char c);
int puts(char *s);
void printf(char *fmt, ...);

void hd_write_sector(uint32 dest, void* src);
void hd_read_sector(void* dest, uint32 src);
extern uint32 maxaddr;

#endif /* stdio.h */
