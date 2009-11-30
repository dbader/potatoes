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
 * Ringbuffer definitions.
 *
 * @author dbader
 * @author $LastChangedBy$
 * @version $Rev$
 */

#ifndef __PM_RINGBUFFER_H
#define __PM_RINGBUFFER_H

#include "../include/types.h"

/**
 * A static circular FIFO buffer.
 */
typedef struct {
        /** the data pointer */
        uint8  *data;

        /** size of data (max value of len) */
        uint32  size;

        /** read position */
        uint32  start;

        /** write position */
        uint32  end;

        /** number of used bytes */
        uint32  len;
} ring_fifo;

ring_fifo* rf_alloc(uint32 size);
ring_fifo* rf_copy(ring_fifo *fifo);
void rf_free(ring_fifo *fifo);
void rf_clear(ring_fifo *fifo);
uint32 rf_getlength(ring_fifo *fifo);
bool rf_isfull(ring_fifo *fifo);
bool rf_isempty(ring_fifo *fifo);
void rf_dump(ring_fifo *fifo);
sint32 rf_write(ring_fifo *fifo, uint8 *buf, uint32 count);
sint32 rf_read(ring_fifo *fifo, uint8 *buf, uint32 count);

#endif /* pm_ringbuffer.h */
