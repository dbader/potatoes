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
 * Headers for string.c
 *
 * @author Dmitriy Traytel
 * @author $LastChangedBy$
 * @version $Rev$
 */
 
extern uint32 strlen(char* str);
extern char* strcpy(char *dest, char *src);
extern char* strncpy(char *dest, char *src, uint32 n);
extern char* strchr(char *str, char ch);
extern char* strcat(char *s1, char *s2);
extern char* strncat(char *s1, char *s2, uint32 n);

extern void* memset(void *dest, uint8 value, uint32 count);
extern void bzero(void *dest, uint32 count);
extern void* memcpy(void *dest, void *src, uint32 count);
