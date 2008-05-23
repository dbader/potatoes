/* $Id: io_keyboard.c 94 2008-05-22 21:16:56Z dtraytel $
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
 * Harddisk-handler
 * 
 * @author Dmitriy Traytel
 * @author $LastChangedBy: dtraytel $
 * @version $Rev: 94 $
 */

#include "../include/stdio.h"
#include "../include/types.h"

#include "../io/io.h"
#include "../io/io_harddisk.h"

void hd_init(){
	struct hd_info hd1;
	outb(0x1F6,0); //select master drive
	outb(0x1F7,0xEC); //identify drive
	repinsw(0x1F0,(uint16*)&hd1,256); //read buffer
	printf("hard disk initialized: \n\t%u cylinders\n\t%u heads\n\t%u sectors per track\n"
			,hd1.apparent_cyl,hd1.apparent_head,hd1.apparent_sector_per_track);
}

void hd_handler(){
	
}
