/* $Id: mm_bitmap.h 266 2009-10-14 08:15:36Z dtraytel $
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
 * Declarations of the functions needed for frame allocation.
 *
 * @author Johannes Schamburger
 * @author $LastChangedBy $
 * @version $Rev $
 */

#include "../include/types.h"
#include "../include/stdio.h"
#include "mm_paging.h"

#ifndef __FRAME_H_
#define __FRAME_H_

void set_frame(uint32 frame_addr);
void clear_frame(uint32 frame_addr);
bool test_frame(uint32 frame_addr);
uint32 first_free_frame();

void alloc_frame(page_t *page, int kernel, int writeable);
void free_frame(page_t *page);

void bitset_test();

#endif /*__FRAME_H_*/
