/* $Id$
      _   _  ____   _____ 
     | | (_)/ __ \ / ____|uint16
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

struct hd_info hd1;
uint32 maxaddr;
volatile bool hd_interrupt = FALSE;

void dump_hd1(){
        int i = 0;

        printf("%{io:} hard disk initialization:"
                        "\n\t%u cylinders"
                        "\n\t%u heads"
                        "\n\t%u sectors per track"
                        "\n%{\t---------------}"
                        "\n\t%d\tmaximal address\n", YELLOW
                        ,hd1.apparent_cyl
                        ,hd1.apparent_head
                        ,hd1.apparent_sector_per_track
                        ,YELLOW
                        ,maxaddr);

        printf("%{\t---------------}\n", YELLOW);

        (hd1.buffer_type==3) ? printf("\tbuffer type:\ttwo way, cache possible\n") : 
                ((hd1.buffer_type==1) ? printf("\tbuffer type:\tone way, single sector\n") : 
                        printf("\tbuffer type:\ttwo way, multiple sectors\n"));

        printf("\tbuffer size:\t%d Bytes\n", hd1.buffer_size*512);      

        printf("\tserial:\t\t");
        for(i=20; i<40; i++)
                putchar(((uint8*)(&hd1))[i]);
        printf("\n");

        printf("\tfirmware:\t");
        for(i=46; i<54; i++)
                putchar(((uint8*)(&hd1))[i]);
        printf("\n");

        printf("\tmodel:\t\t");
        for(i=54; i<94; i++)
                putchar(((uint8*)(&hd1))[i]);
        printf("\n");

        (hd1.lba_dma_flg & 0x100) ? 
                        printf("\tdma support:\tyes\n") : printf("\tdma support:\tno\n");
        (hd1.lba_dma_flg & 0x200) ? 
                        printf("\tlba support:\tyes\n") : printf("\tlba support:\tno\n");        

        printf("\n");
}

/**
 * Waits on hard disk to set the hd_interrupt flag or the drive_ready flag in the status register 
 */
void wait_on_hd_interrupt()
{
        uint8 stat=inb(HDALTBASE + HDALTREG_STAT);
        //printf("0x%x\n", stat);
        while(!hd_interrupt){
                stat=inb(HDALTBASE + HDALTREG_STAT);
                //printf("0x%x\n", stat);
                if ((stat & 0x80) == 0) { //busy bit isn't set
                        inb(HDBASE + HDREG_STAT);
                        break;
                }
                else if (stat & 1) panic("IDE-ERROR"); //error flag
                halt();
        }
        hd_interrupt = FALSE;
}

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
 * Initializes the hard disk drive (IDE 0 master).
 */
void hd_init()
{
        /*outb(HDBASE+HDREG_DRIVE, MASTERDRIVE);
        outb(HDBASE+HDREG_STAT,0x10); //recalibrate
        wait_on_hd_interrupt();
        uint8 stat = inb(HDBASE+HDREG_STAT);
        printf("initial status: 0x%x\n",stat);
        outb(HDBASE + HDREG_DRIVE, MASTERDRIVE); //select master drive
        outb(HDBASE + HDREG_STAT, 0x90); //execute drive diagnostics
        wait_on_hd_interrupt();
        uint8 stat = inb(HDBASE + HDREG_ERR);
        printf("error register: 0x%x\n",stat);
         */
        outb(HDBASE + HDREG_DRIVE, MASTERDRIVE); //select master drive
        //sleep_ticks(1);
        outb(HDBASE + HDREG_STAT, HDCMD_IDENTIFY_DEVICE); //identify device
        if (inb(HDBASE + HDREG_STAT) == 0) {
                panic("NO HARD DRIVE");
        }
        wait_on_hd_interrupt();
        repinsw(HDBASE + HDREG_DATA, (uint16*)&hd1, 256); //read buffer
        maxaddr = get_hdsize()-1;
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
        if (dest > maxaddr) {
                panic("hd: address too large");
        }
        uint8 stat = inb(HDALTBASE + HDALTREG_STAT);
        if (stat & 0x80) {
                //printf("write error %x\n",stat);
                wait_on_hd_interrupt();
        }
        struct address addr = itoaddr(dest);
        outb(HDBASE + HDREG_COUNT, 1);
        outb(HDBASE + HDREG_SEC, addr.sector);
        outb(HDBASE + HDREG_CYL_LOW, addr.cyl);
        outb(HDBASE + HDREG_CYL_HIGH, addr.cyl >> 16);
        outb(HDBASE + HDREG_DRIVE, MASTERDRIVE + addr.head);
        //sleep_ticks(1);
        outb(HDBASE + HDREG_STAT, HDCMD_WRITE); //write sector
        wait_on_hd_interrupt();
        repoutsw(HDBASE + HDREG_DATA, src, 256); //write buffer
}

/**
 * Reads a sector from the hard disk.
 * 
 * @param *dest the pointer where the read data should go to 
 * @param src the address of the sector to be read from
 */
void hd_read_sector(void *dest, uint32 src)
{
        if(src > maxaddr) panic("hd: address too large");
        uint8 stat = inb(HDALTBASE + HDALTREG_STAT);
        if(stat & 0x80){
                //printf("read error %x\n",stat);
                wait_on_hd_interrupt();
        }

        struct address addr = itoaddr(src);

        outb(HDBASE + HDREG_COUNT, 1);
        outb(HDBASE + HDREG_SEC, addr.sector);
        outb(HDBASE + HDREG_CYL_LOW, addr.cyl);
        outb(HDBASE + HDREG_CYL_HIGH, addr.cyl >> 16);
        outb(HDBASE + HDREG_DRIVE, MASTERDRIVE + addr.head);
        //sleep_ticks(1);
        outb(HDBASE + HDREG_STAT, HDCMD_READ); //read sector
        wait_on_hd_interrupt();
        repinsw(HDBASE + HDREG_DATA, dest, 256); //read buffer	
}

/**
 * Handles an hard disk interrupt by setting the hd_interrupt flag.
 */
void hd_handler(){
        uint8 stat = inb(HDBASE + HDREG_STAT);
        //printf("int 0x%x\n", stat);
        if (!(stat & 0x80))
                hd_interrupt = TRUE;
        else if (stat & 1)
                panic("IDE ERROR");
        else 
                panic("NO IDEA WHY");
}
