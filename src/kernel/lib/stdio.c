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
#include "../include/stdio.h"

#include "../io/io_virtual.h"

/**
 * Writes a character to stdout.
 * @param c character to write
 * @return the character written
 */

int putchar(char c)
{
        virt_monitor_putc(get_active_virt_monitor(), c);
        //monitor_putc(c);
        return c;
}

int cputchar(char c, uint8 fg, uint8 bg)
{
        virt_monitor_cputc(get_active_virt_monitor(), c, fg, bg);
}

/**
 * Writes a string to stdout.
 * @bug Does not write a terminating newline character. A change in behavior breaks other things
 *      (namely printf()). Please leave it like that for now.
 * @param s the string
 * @return the number of characters written
 */
int puts(char *s)
{
        return virt_monitor_puts(get_active_virt_monitor(), s);
        //monitor_puts(s);
}

int cputs(char *s, uint8 fg, uint8 bg)
{
        return virt_monitor_cputs(get_active_virt_monitor(), s, fg, bg);
}

/**
 * Prints formatted output. The following format specifiers are supported:
 *      %% - prints the % character.
 *      %i - prints a signed integer.
 *      %d - prints a signed integer.
 *      %u - prints an unsigned integer.
 *      %b - prints an unsigned integer in binary format (base 2)
 *      %o - prints an unsigned integer in octal format (base 8).
 *      %x - prints an unsigned integer in hexadecimal format (base 16).
 *      %c - prints a single character.
 *      %s - prints a string. "(null)" if argument is NULL.
 *      %p - prints a pointer (base 16).
 *      %{ - prints the string until } colored
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
        int color;
        char buf[40];
        
        while ((ch = *fmt++) != '\0')
                if (ch == '%') {
                        ch = *fmt++;
                        switch (ch) {
                        case '{': //print colored until }
                                color = (int)*arg++;
                                while((ch = *fmt++) != '}' && ch != '\0'){
                                        cputchar(ch, color, BLACK);
                                }
                                break;
                        case '%': // print '%' 
                                putchar(ch); 
                                break;
                        case 'i': // signed integer
                        case 'd':
                                puts(itoa((sint32)*arg++, buf, 10));
                                break;
                        case 'u': // unsigned integer
                                puts(itoa((uint32)*arg++, buf, 10));
                                break;
                        case 'o': // octal
                                puts(itoa((uint32)*arg++, buf, 8));
                                break;
                        case 'b': // binary
                                puts(itoa((uint32)*arg++, buf, 2));
                                break;
                        case 'c': // character
                                /* This is a bit peculiar but needed to shut up the
                                 * "cast from pointer to integer of different size"
                                 * compiler warning.
                                 * Code was: putchar((char)*arg++);
                                 */
                                character = (int) *arg++;
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
                        case 'x': // hexadecimal integer
                        case 'p': // pointer
                                puts(itoa((uint32)*arg++, buf, 16));
                                break;
                        }
                } else
                        putchar(ch);
}
