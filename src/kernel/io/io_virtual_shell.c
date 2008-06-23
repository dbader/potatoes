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
 * Shell & monitor interface
 * 
 * @author Dmitriy Traytel
 * @author $LastChangedBy$
 * @version $Rev$
 */

#include "../include/stdio.h"
#include "../include/util.h"

#include "io_virtual.h"

bool inited;
virt_monitor shell;
virt_monitor vmonitor;

void prompt()
{
        printf("user@etios>");
}

char* read_command()
{
        sleep(5);
        inited=0;
        return "\n\"test%d%d\" 4 2\n";
}

void execute(char* s)
{
        printf(s,42,23);
}

/**
 * Initializes the virtual shell.
 */
void start_shell()
{
        shell = new_virt_monitor();
        inited=0;
        while(1){
                prompt();
                execute(read_command());
        }
}

/**
 * Initializes the virtual monitor.
 */
void start_vmonitor()
{
        vmonitor = new_virt_monitor();
        inited=1;
}

//FIXME: PM should provide the right virtual monitor
virt_monitor* get_active_virt_monitor(){
        if(inited) return &vmonitor; else return &shell;
}
