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
 * Basic functions for string manipulation.
 *
 * @author Dmitriy Traytel
 * @author $LastChangedBy$
 * @version $Rev$
 */
#include "../include/const.h"
#include "../include/types.h"
#include "../include/stdio.h"
//#include "../mm/mm.h"

/**
 * Returns the length of a null-terminated string.
 * @param str String to check
 * @return Number of characters preceding the terminating null character.
 */
uint32 strlen(char* str)
{
        uint32 length = 0;
        while (*str++ != '\0')
                length++;
        return length;
}

/**
 * Copies string src to string dest (including terminating \0 character).
 * For stability and security reasons, try to use strncpy() whenever possible.
 * @see strncpy
 * @param dest Destination string
 * @param src Source string
 * @return dest, the destination string 
 */
char* strcpy(char *dest, char *src) 
{
        char *ret = dest;
        while (*src != '\0')
                *dest++ = *src++;
        *dest = '\0';
        return ret;
}

/**
 * Copies at most n characters from src to dest. If src is less than n
 * characters long, the remainder of dest is filled with \0 characters.
 * Otherwise, dest is not terminated.
 * @param dest Destination string
 * @param src Source string
 * @param n Number of bytes to copy at most
 * @return dest, the destination string
 */
char* strncpy(char *dest, char *src, uint32 n) 
{
        char *ret = dest;
        while (n-- > 0) {
                if (*src != '\0')
                        *dest++ = *src++;
                else
                        *dest++ = '\0';
        }
        return ret;
}

/**
 * Locates the first occurence of ch n the string pointed to by str.
 * The terminating '\0' character is considered to be part of the string.
 * @param str the string to search
 * @param ch the character to look for
 * @return the first occurence of ch in str or NULL if not found 
 */
char* strchr(char *str, char ch)
{
        do {
                if (*str == ch)
                        return str;
        } while (*str++ != '\0');

        return NULL;
}

/**
 * Duplicates a string. strdup() allocates sufficient memory for a copy of
 * the string str, copies it and returns a pointer to the copied string.
 * Strings returned by strdup() must be released by calling free().
 * @param str the string to duplicate
 * @return The pointer to the duplicated string or NULL on error
 */
char* strdup(char* str)
{
        char *ret = (char*)malloc_name(strlen(str), "strdup()");
        
        if (ret == NULL)
                return NULL;
        else
                return strcpy(ret, str);
}

/**
 * Concatenates two strings by appending a copy of s2 to the end of s1. 
 * @param s1 the "head"
 * @param s2 the "tail"
 * @return the concatenated string
 */
char* strcat(char *s1, char *s2)
{
        return strcpy(strchr(s1, '\0'), s2);
}

/**
 * Concatenates two strings by appending a copy of s2 to the end of s1. 
 * Not more than n characters are copied from s2.
 * @param s1 the "head"
 * @param s2 the "tail"
 * @param n max number of characters to copy
 * @return the concatenated string
 */
char* strncat(char *s1, char *s2, uint32 n)
{
        return strncpy(strchr(s1, '\0'), s2, n);        
}

/**
 * Tokenizes a string. Take note that strsep() will manipulate both the string pointer
 * **str_ptr points at as well as the contents of the respective string.
 *  
 * Example:
 * 
 *      char path[] = "/usr/share/bin/editor";
 *      char delim[] = "/";
 *      char *tok;
 *      char *copy = strdup(path);
 *      char *work_copy = copy;
 *
 *      do {
 *              printf("strsep(\"%s\") ", work_copy);
 *              tok = strsep(&work_copy, delim);
 *              printf("-> \"%s\"\n", tok);
 *      } while (tok != NULL);
 *
 *      printf("\ncopy = %p\n", copy);
 *      printf("work_copy = %p\n", work_copy);
 *      puts("done.");
 *
 *      free(copy);
 * 
 * @bug The current implementation does not handle multiple delimiters (as specified in the
 *      libc manual). Only the first character in *delims is used for tokenizing the input string.
 * 
 * @param str_ptr Pointer to string to tokenize
 * @param delims String containing all delimiter characters
 * @return The next token or NULL if the end of the input string was reached
 */
char* strsep(char **str_ptr, char *delims)
{
        //_printf("strsep(\"%s\") ", *str_ptr);
        
        if (*str_ptr == NULL)
                return NULL;
        
        char *ret = *str_ptr;      
        char *offs = strchr(*str_ptr, *delims);
        
        if (offs == NULL) {
                *str_ptr = NULL;
                return ret;
        }
        
        *offs = '\0';
        *str_ptr = offs + 1;
        
        return ret;
}

/**
 * Compares two strings.
 * @param s1 String
 * @param s2 String 
 * @return 0 if both strings are equal, >0 if s1 greater than s2, <0 if s1 is less than s2
 */
sint32 strcmp(char *s1, char *s2)
{
        while (*s1 != '\0' &&  *s2 != '\0') {
                if (*s1 != *s2)
                        break;
                s1++;
                s2++;
        }
        return (sint32)(*s1 - *s2);
}

/**
 * Writes count bytes of value value to the memory referenced by dest.
 * @param dest Destination memory
 * @param value Value dest is filled with
 * @param count Number of bytes to write
 * @return dest, the destination memory
 */
void* memset(void *dest, uint8 value, uint32 count) 
{
        void *ret = dest;
        for (int i = 0; i < count; i++)
                *((uint8*)dest++) = value;
        return ret;
}

/**
 * Writes count bytes of value zero to dest.
 * @param dest Destination memory
 * @param count Number of bytes to write
 */
void bzero(void *dest, uint32 count)
{
        memset(dest, 0, count);
}

/**
 * Copies count bytes form src to dest.
 * @param dest Destination memory
 * @param src Source memory
 * @param count Number of bytes to copy
 * @return dest, the destination memory
 */
void* memcpy(void *dest, void *src, uint32 count)
{
        void *ret = dest;
        for (int i = 0; i < count; i++)
                *((uint8*)dest++) = *((uint8*)src++);
        return ret;
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
        
        while ((ch = *fmt++) != '\0')
                if (ch != '%')
                        putc(ch);
                else {
                        ch = *fmt++;
                        switch (ch) {
                        case '%': // print '%' 
                                putc(ch); 
                                break;
                        case 'i': // signed integer
                        case 'd':
                                puti((sint32)*arg++);
                                break;
                        case 'u': // unsigned integer
                                puti((uint32)*arg++);
                                break;
                        case 'c': // character
                                putc((char)*arg++[0]);
                                break;
                        case 's': // string
                                if (*arg != NULL) {
                                        while((ch = *(*arg)++) != '\0')
                                                putc(ch);
                                } else {
                                        putc('('); //FIXME: print a string
                                        putc('n');
                                        putc('u');
                                        putc('l');
                                        putc('l');
                                        putc(')');
                                }
                                *arg++;
                                break;
                        case 'p': // pointer
                                puti((uint32)*arg++);
                                break;
                        }
                }
}
