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
 * FS main.
 * 
 * @author Vincenz Doelle
 * @author $LastChangedBy$
 * @version $Rev$
 */

#include "../include/const.h"
#include "../include/types.h"

#include "fs_const.h"
#include "fs_types.h"
#include "fs_main.h"

extern void reset_bmap();
extern void init_super_block();
extern void load_super_block();

extern void panic(char *msg);

void fs_init()
{
        //panic("FS says 'foobar'!");
}

bool fs_shutdown()
{
        return TRUE;   
}

bool fs_load()
{
        //TODO
        load_super_block();
        //load bmap
        
        //load root dir
        return TRUE;
}

bool fs_create()
{
        //TODO
        init_super_block();
        reset_bmap();
        
        //create root dir
        
        return TRUE;
}


