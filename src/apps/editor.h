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
 * Simple Potatoes/Etios EDitor header.
 *
 * @author Dmitriy Traytel
 * @author $LastChangedBy$
 * @version $Rev$
 */
#ifndef __EDITOR_H
#define __EDITOR_H

#include "../kernel/include/debug.h"

typedef struct line {
        int num_chars;
        int offset;
        unsigned char linewidth;
        struct line *next;
        struct line *prev;
} line;

#define NEXT_LINE \
        actualline->next = _malloc(sizeof(line));\
        if(str[pos] == '\n') {\
                actualline->linewidth = 80;\
                if(actualline->prev->linewidth == 8) {\
                        actualline->offset = actualline->prev->offset;\
                }\
        } else {\
                actualline->linewidth = 8;\
                actualline->offset = actualline->prev->offset + actualline->num_chars + 8 - actualline->num_chars % 8;\
        }\
        actualline->next->prev = actualline;\
        actualline = actualline->next;\
        actualline->num_chars = 0;\
        actualline->offset = 0;\
        actualline->next = NULL;

#define PREV_LINE \
        line *temp = actualline;\
        actualline = actualline->prev;\
        actualline->next = temp->next;\
        if(temp->next!=NULL) {\
                temp->next->prev = actualline;\
        }\
        _free(temp);\
        if(actualline->linewidth == 8) {\
                actualline->offset=0;\
        }\
        for(int i = 0;\
        i< actualline->linewidth - actualline->offset - (actualline->num_chars % actualline->linewidth); i++) {\
                _printf("\b");\
        }

#define DUMP_LINES \
        dprintf("NUM_CHARS\tOFFSET\t\tTYPE\n");\
        for(line* temp = startline; temp != NULL; temp = temp->next) {\
                dprintf("%d\t\t%d\t\t%s\n",\
                        temp->num_chars,\
                        temp->offset,\
                        (temp->prev->linewidth == 80)? "\\n" : "\\t");\
        }

#endif /*__EDITOR_H*/
