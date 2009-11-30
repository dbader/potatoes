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
#include "stdarg.h"

enum colors {
        BLACK = 0x0,
        BLUE = 0x1,
        GREEN = 0x2,
        CYAN = 0x3,
        RED = 0x4,
        VIOLET = 0x5,
        ORANGE = 0x6,
        LIGHTGREY = 0x7,
        DARKGREY = 0x8,
        LIGHTBLUE = 0x9,
        LIGHTGREEN = 0xA,
        TURQUOISE = 0xB,
        PINK = 0xC,
        MAGENTA = 0xD,
        YELLOW = 0xE,
        WHITE = 0xF,
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
int cputchar(char c, uint8 fg, uint8 bg);
int puts(char *s);
int cputs(char *s, uint8 fg, uint8 bg);
int vsnprintf(char *s, int n, char *format, va_list ap);
int snprintf(char *buf, int size, char *fmt, ...);
void printf(char *fmt, ...);

void hd_write_sector(uint32 dest, void* src);
void hd_read_sector(void* dest, uint32 src);
extern uint32 maxaddr;

#endif /* stdio.h */
