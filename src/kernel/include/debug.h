/* $Id: debug.h 266 2009-10-14 08:15:36Z dtraytel $
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
 * Debugging utility functions. This is work in progress so far. I plan on implementing
 * a small toolbox of useful debugging functions, eg debug printf, stack crawling etc.
 *
 * @author dbader
 * @author $LastChangedBy: dtraytel $
 * @version $Rev: 266 $
 */

#ifndef __DEBUG_H
#define __DEBUG_H

#include "stdio.h"
#include "../io/io.h"

#define dprintf printf
#define dprint_separator() dprintf("#{GRE}--------------------------------------------------------------------------------##");
#define fs_dprintf(a, ...) ((void)0)
//#define fs_dprintf dprintf("%s ln %d%{:} ", __FILE__, __LINE__, RED); dprintf

#define SHORTCUT_CTRL(ch, func) add_shortcut(TRUE, FALSE, ch, func); \
        printf("\tCTRL + %c ==> %s()\n", ch, #func)

#define SHORTCUT_SUPER(ch, func) add_shortcut(FALSE, TRUE, ch, func); \
        printf("\tSUPER + %c ==> %s()\n", ch, #func)

#define SHORTCUT_CTRL_SUPER(ch, func) add_shortcut(TRUE, TRUE, ch, func); \
        printf("\tCTRL + SUPER + %c ==> %s()\n", ch, #func)

#endif /* debug.h */
