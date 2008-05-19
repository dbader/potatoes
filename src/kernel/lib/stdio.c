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

/* TODO: These are just dummy declarations until we have a proper interface for monitor
 * output, ie monitor_write(char c) */
void putc_col(char ch, uint8 fg, uint8 bg);
void puts_col(char *str, uint8 fg, uint8 bg);
void putc(char ch);
void puts(char *str);
void puti(sint32 x);
void puthex(uint8 ch);

/**
 * Writes a character to stdout.
 * @param c character to write
 */
void putchar(char c)
{
        putc(c);
}

/**
 * Prints formatted output. The following format specifiers are supported:
 *      %% - prints the % character.
 *      %i - prints a signed integer.
 *      %d - prints a signed integer.
 *      %u - prints an unsigned integer.
 *      %c - prints a single character.
 *      %s - prints a string. "(null)" if argument is NULL.
 *      %p - prints a pointer.
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
        
        while ((ch = *fmt++) != '\0')
                if (ch == '%') {
                        ch = *fmt++;
                        switch (ch) {
                        case '%': // print '%' 
                                putchar(ch); 
                                break;
                        case 'i': // signed integer
                        case 'd':
                                puti((sint32)*arg++);
                                break;
                        case 'u': // unsigned integer
                                puti((uint32)*arg++);
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
                                        puts("(null)");
                                }
                                *arg++;
                                break;
                        case 'p': // pointer
                                puti((uint32)*arg++);
                                break;
                        }
                } else
                        putchar(ch);
}
