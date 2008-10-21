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
 * Simple Potatoes/Etios EDitor header.
 *
 * @author Dmitriy Traytel
 * @author $LastChangedBy$
 * @version $Rev$
 */
#ifndef __EDITOR_H
#define __EDITOR_H

typedef struct line {
        int num_chars;
        unsigned char linewidth;
        struct line *next;
        struct line *prev;
} line;

#define NEXT_LINE \
        if(str[pos] == '\n') {\
                actualline->linewidth = 80;\
        } else {\
                actualline->linewidth = 8;\
        }\
        actualline->next = _malloc(sizeof(line));\
        actualline->next->prev = actualline;\
        actualline = actualline->next;\
        actualline->num_chars=0;\
        actualline->next = NULL;

#define PREV_LINE \
        actualline = actualline->prev;\
        _free(actualline->next);\
        actualline->next = NULL;\
        for(int i = 0;\
                i< actualline->linewidth - (actualline->num_chars % actualline->linewidth); i++) {\
                _printf("\b");\
        }

#endif /*__EDITOR_H*/
