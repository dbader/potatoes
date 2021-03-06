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
 * Ring buffer implementation. Mainly used by the process management stdin queue.
 *
 * @author dbader
 * @author $LastChangedBy$
 * @version $Rev$
 */

#include "../include/ringbuffer.h"
#include "../include/types.h"
#include "../include/const.h"
#include "../include/assert.h"
#include "../include/stdio.h"
#include "../include/string.h"
#include "../include/stdlib.h"

/**
 * Allocates and initializes a new ring_buffer.
 * @param size max size of the buffer
 * @return pointer to new ring_buffer or NULL on error
 */
ring_fifo* rf_alloc(uint32 size)
{
        ring_fifo *fifo = (ring_fifo*) mallocn(sizeof(ring_fifo),"rf_alloc: ring_fifo");

        if (fifo == NULL)
                return NULL;

        fifo->size = size;
        fifo->data = (uint8*) mallocn(size,"rf_alloc: data");

        if (fifo->data == NULL)
                return NULL;

        rf_clear(fifo);

        return fifo;
}

/**
 * Copies the given ring_buffer into a new allocated copy.
 * @param fifo the ring_buffer to copy from
 * @return pointer to the new copy of the given ring_buffer
 */
ring_fifo* rf_copy(ring_fifo *fifo)
{
        ring_fifo *ret = rf_alloc(fifo->size);
        memcpy(ret->data, fifo->data, fifo->size);
        ret->start = fifo->start;
        ret->end = fifo->end;
        ret->len = fifo->len;
        return ret;
}

/**
 * Destroys a previously allocated ring_buffer.
 * @param fifo the ring buffer to free
 */
void rf_free(ring_fifo *fifo)
{
        ASSERT(fifo != NULL)
        free(fifo->data);
        free(fifo);
}

/**
 * Clears a ring buffer.
 * @param fifo the buffer
 */
void rf_clear(ring_fifo *fifo)
{
        ASSERT(fifo != NULL)
        fifo->start = 0;
        fifo->end = 0;
        fifo->len = 0;
}

/**
 * Returns the buffer's number of used bytes.
 * @param fifo the buffer
 * @return Number of used bytes
 */
uint32 rf_getlength(ring_fifo *fifo)
{
        ASSERT(fifo != NULL)
        return fifo->len;
}

/**
 * Checks whether the given ring_buffer is full.
 * @param fifo the buffer to check
 * @return TRUE if at least one byte of storage is available, FALSE if not.
 */
bool rf_isfull(ring_fifo *fifo)
{
        ASSERT(fifo != NULL)
        return (fifo->len == fifo->size);
}

/**
 * Checks whether the given ring_buffer is empty.
 * @param fifo the buffer to check
 * @return TRUE if the buffer contains at least one byte of data, false if not
 */
bool rf_isempty(ring_fifo *fifo)
{
        ASSERT(fifo != NULL)
        return (fifo->len == 0);
}

/**
 * Prints information about the buffer to stdout
 * @param fifo the buffer
 */
void rf_dump(ring_fifo *fifo)
{
        ASSERT(fifo != NULL)
        printf("ring_fifo @ %p:\n\tdata = %p\n\tsize = %d\n\tstart = %d\n\tend = %d\n\tlen = %d\n",
               fifo, fifo->data, fifo->size, fifo->start, fifo->end, fifo->len);
}

/**
 * Writes data into a ring_buffer. If the buffer gets filled up less than
 * count bytes might be written.
 * @param fifo ring_buffer to write to
 * @param buf buffer to read from
 * @param count number of bytes to write
 * @return Number of bytes written, -1 on error.
 */
sint32 rf_write(ring_fifo *fifo, uint8 *buf, uint32 count)
{
        if (fifo == NULL || buf == NULL)
                return -1;

        sint32 written = 0;

        while (count-- > 0) {
                if (fifo->len == fifo->size) {
                        printf("rf_write: buffer is full.");
                        return written;
                }

                fifo->data[fifo->end] = *buf++;
                fifo->end = (fifo->end + 1) % fifo->size;
                fifo->len++;
                written++;
        }

        return written;
}

/**
 * Reads data from a ring_buffer. Less than count bytes might be read if
 * the buffer becomes empty.
 * @param fifo ring_buffer to read from
 * @param buf buffer to write to
 * @param count number of bytes to write
 * @return Number of bytes read, -1 on error
 */
sint32 rf_read(ring_fifo *fifo, uint8 *buf, uint32 count)
{
        if (fifo == NULL || buf == NULL)
                return -1;

        sint32 read = 0;

        while (count-- > 0) {
                if (fifo->len == 0) {
                        return read;
                }

                *buf++ = fifo->data[fifo->start];
                fifo->start = (fifo->start + 1) % fifo->size;
                fifo->len--;
                read++;
        }

        return read;
}
