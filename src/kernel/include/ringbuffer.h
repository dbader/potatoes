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
