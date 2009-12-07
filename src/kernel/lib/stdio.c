/* $Id$
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
#include "../include/stdarg.h"

#include "../io/io_virtual.h"
#include "../pm/pm_main.h"


/** 
 * FIXME: 
 * This is used to select a vmonitor for printing. Simply using the
 * active monitor does not work, as kernel messages get printed to the vmonitors
 * of other processes.
 * Always using the kernel_proc vmonitor also fails because kernel_proc is
 * initialized very late during boot which causes messages to be lost.
 * 
 * I feel this is quite hackish and should be changed as soon as anybody
 * comes up with a proper solution. 
 */
#define SELECT_VMONITOR() ((kernel_proc == NULL) ? get_active_virt_monitor() : kernel_proc->vmonitor)

/**
 * Writes a character to stdout.
 * @param c character to write
 * @return the character written
 */
int putchar(char c)
{        
        virt_monitor_putc(SELECT_VMONITOR(), c);
        //monitor_putc(c);
        return c;
}

int cputchar(char c, uint8 fg, uint8 bg)
{
        virt_monitor_cputc(SELECT_VMONITOR(), c, fg, bg);
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
        return virt_monitor_cputs(SELECT_VMONITOR(), s, WHITE, BLACK);
        //monitor_puts(s);
}

int cputs(char *s, uint8 fg, uint8 bg)
{
        return virt_monitor_cputs(SELECT_VMONITOR(), s, fg, bg);
}

int vsnprintf(char *s, int n, char *format, va_list ap)
{      
        if (format == NULL) {
                return 0;
        }

        char ch;
        char buf[40];
        char null_msg[] = "(null)";
        int written = 0;      
        char *buf_ptr;
        int int_arg;
        char char_arg;
        char *str_arg;

        while (((ch = *format++) != '\0') && (written++ < n-1))
                if (ch == '%') {
                        ch = *format++;
                        switch (ch) {
                        //case '{': int_arg = va_arg(ap, int);
                        case '%': // print '%'
                                *(s++) = ch;
                                break;
                        case 'i': // signed integer
                        case 'd':
                        case 'u': // FIXME: unsigned should actually print unsigned values. (dbader)
                                int_arg = va_arg(ap, int);
                                itoa(int_arg, buf, 10);
                                buf_ptr = buf;
                                while (*buf_ptr && written++ < n-1)
                                        *(s++) = *(buf_ptr++);
                                break;
                        case 'o': // octal
                                int_arg = va_arg(ap, int);
                                itoa(int_arg, buf, 8);
                                buf_ptr = buf;
                                while (*buf_ptr && written++ < n-1)
                                        *(s++) = *(buf_ptr++);
                                break;
                        case 'b': // binary
                                int_arg = va_arg(ap, int);
                                itoa(int_arg, buf, 2);
                                buf_ptr = buf;
                                while (*buf_ptr && written++ < n-1)
                                        *(s++) = *(buf_ptr++);
                                break;
                        case 'c': // character (note that chars get promoted to int when passed through "...")
                                int_arg = va_arg(ap, int);
                                *(s++) = (char)int_arg;
                                break;
                        case 's': // string
                                str_arg = va_arg(ap, char*);
                                                                
                                if (str_arg != NULL) {
                                        buf_ptr = str_arg;
                                } else {
                                        buf_ptr = null_msg;
                                        
                                }
                                while (*buf_ptr && written < n-1) {
                                        *(s++) = *(buf_ptr++);
                                }
                                break;
                        case 'x': // hexadecimal integer
                        case 'p': // pointer
                                int_arg = va_arg(ap, int);
                                itoa(int_arg, buf, 16);
                                buf_ptr = buf;
                                while (*buf_ptr && written++ < n-1)
                                        *(s++) = *(buf_ptr++);
                                break;
                        }
                } else {
                        *(s++) = ch;
                }
        
        *(s++) = '\0';
        return (++written);
}

int snprintf(char *buf, int size, char *fmt, ...)
{
        va_list arg_list;
        va_start(arg_list, fmt);
        vsnprintf(buf, size, fmt, arg_list);
        va_end(arg_list);
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
        char buf[255];
        va_list arg_list;
        va_start(arg_list, fmt);
        
        vsnprintf(buf, sizeof(buf), fmt, arg_list);
        puts(buf);
        
        va_end(arg_list);
        
//       
//        if (fmt == NULL)
//                return;
//
//        char **arg = &fmt + 1;
//        char ch;
//        int character;
//        int color;
//        char buf[40];
//
//        while ((ch = *fmt++) != '\0')
//                if (ch == '%') {
//                        ch = *fmt++;
//                        switch (ch) {
//                        case '{': //print colored until }
//                                color = (int) * arg++;
//                                while ((ch = *fmt++) != '}' && ch != '\0') {
//                                        cputchar(ch, color, BLACK);
//                                }
//                                if (ch = '\0') fmt--; //to break out from the while loop
//                                break;
//                        case '%': // print '%'
//                                putchar(ch);
//                                break;
//                        case 'i': // signed integer
//                        case 'd':
//                                puts(itoa((sint32)*arg++, buf, 10));
//                                break;
//                        case 'u': // unsigned integer
//                                puts(itoa((uint32)*arg++, buf, 10));
//                                break;
//                        case 'o': // octal
//                                puts(itoa((uint32)*arg++, buf, 8));
//                                break;
//                        case 'b': // binary
//                                puts(itoa((uint32)*arg++, buf, 2));
//                                break;
//                        case 'c': // character
//                                /* This is a bit peculiar but needed to shut up the
//                                 * "cast from pointer to integer of different size"
//                                 * compiler warning.
//                                 * Code was: putchar((char)*arg++);
//                                 */
//                                character = (int) * arg++;
//                                putchar((char)character);
//                                break;
//                        case 's': // string
//                                if (*arg != NULL) {
//                                        while ((ch = *(*arg)++) != '\0')
//                                                putchar(ch);
//                                } else {
//                                        puts("(null)");
//                                }
//                                *arg++;
//                                break;
//                        case 'x': // hexadecimal integer
//                        case 'p': // pointer
//                                puts(itoa((uint32)*arg++, buf, 16));
//                                break;
//                        }
//                } else
//                        putchar(ch);
}
