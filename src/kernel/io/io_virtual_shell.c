/* $Id: io_timer.c 119 2008-06-09 22:15:34Z dtraytel $
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
 * Shell & monitor interface
 * 
 * @author Dmitriy Traytel
 * @author $LastChangedBy: dtraytel $
 * @version $Rev: 119 $
 */

#include "../include/stdio.h"
#include "../include/util.h"

#include "io_virtual.h"

virt_monitor shell;
virt_monitor vmonitor;

void prompt()
{
        printf("user@etios>\n");
}

char* read_command()
{
        sleep_ticks(30);
        return "printf \"test%d%d\" 4 2\n";
}

void execute(char* s)
{
        printf(s);
}

/**
 * Initializes the virtual shell.
 */
void start_shell()
{
        shell = new_virt_monitor();
}

/**
 * Initializes the virtual monitor.
 */
void start_vmonitor()
{
        vmonitor = new_virt_monitor();
}

//FIXME: PM should provide the right virtual monitor
virt_monitor* get_active_virt_monitor(){
        return &vmonitor;
}
