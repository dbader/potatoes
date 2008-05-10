/* $Id: block_dev.h 37 2008-05-02 22:59:03Z vdoelle $
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
 * Basic definitions of all based functions.
 *
 * @author Vincenz Doelle
 * @author $LastChangedBy: vdoelle $
 * @version $Rev: 37 $
 */
 
#ifndef BLOCK_MGR_
#define BLOCK_MGR_

extern uint32 read_block(block_nr block, uint16 *buf, uint16 num_bytes);

extern uint32 write_block(block_nr block, uint16 *buf, uint16 num_bytes);

extern uint8 free_block(block_nr block);

extern uint8 alloc_block(block_nr near_to_block);

extern uint32 new_block(); 

#endif /*BLOCK_MGR_*/
