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
 * Standard I/O functions.
 *
 * @author dbader
 * @author $LastChangedBy$
 * @version $Rev$
 */

#include "../include/types.h"
#include "../include/const.h"
#include "../include/string.h"

/* TODO: These are just dummy declarations until we have a proper interface for monitor
 * output, ie monitor_write(char c) */
void monitor_cputc(char ch, uint8 fg, uint8 bg);
void monitor_cputs(char *str, uint8 fg, uint8 bg);
void monitor_putc(char ch);
void monitor_puts(char *str);
void monitor_puti(sint32 x);
void monitor_puthex(uint8 ch);

/**
 * Writes a character to stdout.
 * @param c character to write
 */
void putchar(char c)
{
        monitor_putc(c);
}

/**
 * Prints formatted output. The following format specifiers are supported:
 *      %% - prints the % character.
 *      %i - prints a signed integer.
 *      %d - prints a signed integer.
 *      %u - prints an unsigned integer.
 *      %o - prints an unsigned integer in octal format (base 8).
 *      %x - prints an unsigned integer in hexadecimal format (base 16).
 *      %c - prints a single character.
 *      %s - prints a string. "(null)" if argument is NULL.
 *      %p - prints a pointer (base 16).
 * All other format specifiers are ignored.
 * @param fmt format string
 * @param ... variable number of arguments
 */
void printf(char *fmt, ...)
{
        if (fmt == NULL)
                return;
        
        char **arg = &fmt + 1;
        char ch; 
        int character;
        char buf[40];
        
        while ((ch = *fmt++) != '\0')
                if (ch == '%') {
                        ch = *fmt++;
                        switch (ch) {
                        case '%': // print '%' 
                                putchar(ch); 
                                break;
                        case 'i': // signed integer
                        case 'd':
                                monitor_puts(itoa((sint32)*arg++, buf, 10));
                                break;
                        case 'u': // unsigned integer
                                monitor_puts(itoa((uint32)*arg++, buf, 10));
                                break;
                        case 'o':
                                monitor_puts(itoa((uint32)*arg++, buf, 8));
                                break;
                        case 'c': // character
                                /* This is a bit peculiar but needed to shut up the
                                 * "cast from pointer to integer of different size"
                                 * compiler warning.
                                 * Code was: putchar((char)*arg++);
                                 */
                                character = (int)*arg++;
                                putchar((char)character);
                                break;
                        case 's': // string
                                if (*arg != NULL) {
                                        while ((ch = *(*arg)++) != '\0')
                                                putchar(ch);
                                } else {
                                        monitor_puts("(null)");
                                }
                                *arg++;
                                break;
                        case 'x': // hexadecimal integer
                        case 'p': // pointer
                                monitor_puts(itoa((uint32)*arg++, buf, 16));
                                break;
                        }
                } else
                        putchar(ch);
}
