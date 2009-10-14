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
 * Harddisk-handler
 *
 * @author Dmitriy Traytel
 * @author $LastChangedBy$
 * @version $Rev$
 */

#include "../include/stdio.h"
#include "../include/types.h"
#include "../include/const.h"
#include "../include/init.h"
#include "../include/util.h"

#include "../io/io.h"
#include "../io/io_harddisk.h"

/**
 *  Our single hard disk struct. etiOS is handling only one hard disk device.
 */
struct hd_info hd1;

/**
 *  Signals a pending hard disk interrupt.
 */
volatile bool hd_interrupt = FALSE;

/**
 * Printing some hard disk informations, taken from the hd1 struct.
 */
void dump_hd1()
{
        int i = 0;

        printf("#{YEL}io:## hard disk initialization:"
               "\n\t%u cylinders"
               "\n\t%u heads"
               "\n\t%u sectors per track"
               "\n\t%u bytes per sector"
               "\n#{YEL}\t---------------##"
               "\n\t%d\tmaximal address\n"
               , hd1.apparent_cyl
               , hd1.apparent_head
               , hd1.apparent_sector_per_track
               , hd1.bytes_per_sector
               , get_hdsize());

        printf("#{YEL}\t---------------##\n");

        (hd1.buffer_type == 3) ? printf("\tbuffer type:\ttwo way, cache possible\n") :
        ((hd1.buffer_type == 1) ? printf("\tbuffer type:\tone way, single sector\n") :
         printf("\tbuffer type:\ttwo way, multiple sectors\n"));

        printf("\tbuffer size:\t%d Bytes\n", hd1.buffer_size*512);

        printf("\tserial:\t\t");
        for (i = 20; i < 40; i += 2) {
                putchar(((uint8*)(&hd1))[i+1]);
                putchar(((uint8*)(&hd1))[i]);
        }
        printf("\n");

        printf("\tfirmware:\t");
        for (i = 46; i < 54; i += 2) {
                putchar(((uint8*)(&hd1))[i+1]);
                putchar(((uint8*)(&hd1))[i]);
        }
        printf("\n");

        printf("\tmodel:\t\t");
        for (i = 54; i < 94; i += 2) {
                putchar(((uint8*)(&hd1))[i+1]);
                putchar(((uint8*)(&hd1))[i]);
        }
        printf("\n");

        (hd1.lba_dma_flg & 0x100) ?
        printf("\tdma support:\tyes\n") : printf("\tdma support:\tno\n");
        (hd1.lba_dma_flg & 0x200) ?
        printf("\tlba support:\tyes\n") : printf("\tlba support:\tno\n");

        printf("\n");
}

/**
 * Selecting a head of the masterdrive.
 *
 * @param head the number of the head to be selected
 */
void select_masterdrive(uint8 head)
{
        outb(HDBASE + HDREG_DRIVE, MASTERDRIVE | head);
//TODO: see if this is necessary
//        This is a bit ugly. It is used as an short delay (idea taken from an osdev-tutorial)
//        inb(HDALTBASE + HDALTREG_STAT);
//        inb(HDALTBASE + HDALTREG_STAT);
//        inb(HDALTBASE + HDALTREG_STAT);
//        inb(HDALTBASE + HDALTREG_STAT);
//        inb(HDALTBASE + HDALTREG_STAT);
}

/**
 * Waits on hard disk to set the hd_interrupt flag or the drive_ready flag in the status register
 *
 * @param str should be the source of the function call for debugging issues
 */
void wait_on_hd_interrupt(char* str)
{
        uint8 stat = inb(HDALTBASE + HDALTREG_STAT);

        while (!hd_interrupt) {
                stat = inb(HDALTBASE + HDALTREG_STAT);
                if ((stat & 0x80) == 0) { //busy bit isn't set
                        inb(HDBASE + HDREG_STAT);
                        break;
                } else if (stat & 1) {
                        printf("error code(%s): 0x%x\n", str, inb(HDBASE + HDREG_ERR));
                }
                halt();
        }
        hd_interrupt = FALSE;
}

/**
 * Converts a linear block number into a valid CHS-address
 *
 * @param iaddr the logical block number
 * @return the CHS-address struct
 */
struct address itoaddr(uint32 iaddr) 
{
        struct address addr;
        addr.sector = iaddr % hd1.apparent_sector_per_track + 1;
        iaddr = (iaddr - addr.sector + 1) / hd1.apparent_sector_per_track;
        addr.head = iaddr % hd1.apparent_head;
        addr.cyl = (iaddr - addr.head) / hd1.apparent_head;
        return addr;
}

/**
 * Returns the size of the hard disk.
 *
 * @return size of the master hard disk in sectors
 */
uint32 get_hdsize()
{
        return (hd1.apparent_capacity[1] << 16) + hd1.apparent_capacity[0];
}

/**
 * Initializes the hard disk drive.
 */
void hd_init()
{
/*TODO: see if this is necessary
        outb(HDBASE+HDREG_DRIVE, MASTERDRIVE);
        outb(HDBASE+HDREG_STAT,0x10); //recalibrate
        wait_on_hd_interrupt();
        uint8 stat = inb(HDBASE+HDREG_STAT);
        printf("initial status: 0x%x\n",stat);
        outb(HDBASE + HDREG_DRIVE, MASTERDRIVE); //select master drive
        outb(HDBASE + HDREG_CMD, HDCMD_EXEC_DRIVE_DIAG); //execute drive diagnostics
        wait_on_hd_interrupt();
        stat = inb(HDBASE + HDREG_ERR);
        printf("error register: 0x%x\n",stat);
*/

        uint8 stat = inb(HDBASE + HDREG_STAT);
        if (stat == HDSTATE_FLOATINGBUS) { 
                panic("hd_init: Floating Bus");
        }
        if (stat & HDSTATE_NOTREADY) { 
                wait_on_hd_interrupt("hd_init: not ready");
        }
        select_masterdrive(0);
        outb(HDBASE + HDREG_CMD, HDCMD_IDENTIFY_DEVICE); // identify device
        if (inb(HDBASE + HDREG_STAT) == 0) {
                panic("hd_init: NO HARD DRIVE");
        }
        wait_on_hd_interrupt("hd_init");
        repinsw(HDBASE + HDREG_DATA, (uint16*)&hd1, 256); // read buffer
        dump_hd1();
}

/**
 * Writes a sector to the hard disk.
 *
 * @param dest the address of the sector to be written to
 * @param *src the pointer where data should be written from
 */
void hd_write_sector(uint32 dest, void *src)
{
        if (dest > get_hdsize()) {
                panic("hd(write): address too large");
        }
        if (inb(HDALTBASE + HDALTREG_STAT) & HDSTATE_NOTREADY) {
                wait_on_hd_interrupt("write not ready");
        }
        struct address addr = itoaddr(dest);
        outb(HDBASE + HDREG_COUNT, 1);
        outb(HDBASE + HDREG_SEC, addr.sector);
        outb(HDBASE + HDREG_CYL_LOW, addr.cyl);
        outb(HDBASE + HDREG_CYL_HIGH, addr.cyl >> 16);
        select_masterdrive(addr.head);

        outb(HDBASE + HDREG_CMD, HDCMD_WRITE); // write sector
        wait_on_hd_interrupt("write");
        repoutsw(HDBASE + HDREG_DATA, src, 256); // write buffer

/**
 * The EEEPC needs a cache flush after a write operation.
 */
#ifndef EEEPC
        outb(HDBASE + HDREG_CMD, HDCMD_FLUSH_CACHE);
        wait_on_hd_interrupt("flush");
#endif
}

/**
 * Reads a sector from the hard disk.
 *
 * @param *dest the pointer where the read data should go to
 * @param src the address of the sector to be read from
 */
void hd_read_sector(void *dest, uint32 src)
{
        if (src > get_hdsize()) {
                panic("hd(read): address too large");
        }
        if (inb(HDALTBASE + HDALTREG_STAT) & HDSTATE_NOTREADY) {
                wait_on_hd_interrupt("read not ready");
        }

        struct address addr = itoaddr(src);

        outb(HDBASE + HDREG_COUNT, 1);
        outb(HDBASE + HDREG_SEC, addr.sector);
        outb(HDBASE + HDREG_CYL_LOW, addr.cyl);
        outb(HDBASE + HDREG_CYL_HIGH, addr.cyl >> 16);
        select_masterdrive(addr.head);
        outb(HDBASE + HDREG_CMD, HDCMD_READ); //read sector
        wait_on_hd_interrupt("read");
        repinsw(HDBASE + HDREG_DATA, dest, 256); //read buffer
}

/**
 * Handles an hard disk interrupt by setting the hd_interrupt flag.
 */
void hd_handler()
{
        uint8 stat = inb(HDBASE + HDREG_STAT);
        if (!(stat & 0x80)) {
                hd_interrupt = TRUE;
        } else if (stat & 1) {
                panic("hd_handler: IDE ERROR");
        } else {
                panic("hd_handler: UNKNOWN ERROR");
        }
}
