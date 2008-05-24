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

#include "../io/io.h"
#include "../io/io_harddisk.h"

struct hd_info hd1;
uint32 maxaddr;
volatile bool hd_interrupt = FALSE;

void wait_on_hd_interrupt()
{
	while(!hd_interrupt){}
	hd_interrupt = FALSE;
}

struct address itoaddr(uint32 iaddr)
{
	struct address adr;
	adr.sector = iaddr % hd1.apparent_sector_per_track + 1;
	iaddr = (iaddr - adr.sector + 1) / hd1.apparent_sector_per_track;
	adr.head = iaddr % hd1.apparent_head;
	adr.cyl = (iaddr - adr.head) / hd1.apparent_head;
	return adr;
}

uint32 get_hdsize()
{
	return ((hd1.apparent_cyl*hd1.apparent_head+hd1.apparent_head)*hd1.apparent_sector_per_track + hd1.apparent_sector_per_track - 1); 
}

void hd_init()
{
	outb(0x1F6,0); //select master drive
	outb(0x1F7,0xEC); //identify drive
	wait_on_hd_interrupt();
	repinsw(0x1F0,(uint16*)&hd1,256); //read buffer
	maxaddr = get_hdsize();
	printf("hard disk initialized: \n\t%u cylinders\n\t%u heads\n\t%u sectors per track\n\t---------------\n\t%d\tmaximal address\n"
			,hd1.apparent_cyl,hd1.apparent_head,hd1.apparent_sector_per_track,maxaddr);
}

void hd_write_sector(uint32 dest, void *src)
{
	if(dest > maxaddr) return;
	struct address addr = itoaddr(dest);
	outb(0x1F2,1);
	outb(0x1F3,addr.sector);
	outb(0x1F4,addr.cyl);
	outb(0x1F5,addr.cyl >> 16);
	outb(0x1F6,0xA0+addr.head);
	outb(0x1F7,0x30); //write sector
	repoutsw(0x1F0,src,256); //write buffer
	wait_on_hd_interrupt();
}

void hd_read_sector(void *dest, uint32 src)
{
	if(src > maxaddr) return;
	struct address addr = itoaddr(src);
	outb(0x1F2,1);
	outb(0x1F3,addr.sector);
	outb(0x1F4,addr.cyl);
	outb(0x1F5,addr.cyl >> 16);
	outb(0x1F6,0xA0+addr.head);
	outb(0x1F7,0x20); //read sector
	wait_on_hd_interrupt();
	repinsw(0x1F0,dest,256); //read buffer	
}

void hd_handler(){
	if (inb(0x1F7) & 0x40) hd_interrupt = TRUE;
	else panic("IDE ERROR");
}
