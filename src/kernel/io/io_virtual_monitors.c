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
#include "../include/stdlib.h"
#include "../include/string.h"
#include "../include/util.h"

#include "io_virtual.h"

uint16 active_monitor = -1;
uint16 maxvmonitor = -1;
virt_monitor *vmonitors;
uint16 num_vmonitor_limit = 1000;
char *vmonitor_names;

void switch_monitor_up()
{
        if(active_monitor<maxvmonitor) active_monitor++;
}

void switch_monitor_down()
{
        if(active_monitor>0) active_monitor--;
}

void init_vmonitors()
{
        vmonitor_names = (char*) malloc(num_vmonitor_limit*81*sizeof(char));
        bzero(vmonitor_names, num_vmonitor_limit*81*sizeof(char));
        vmonitors = (virt_monitor*)malloc(num_vmonitor_limit*sizeof(virt_monitor));
        start_vmonitor("DEBUG MONITOR");
}

/**
 * Initializes the virtual monitor.
 */
void start_vmonitor(char *name)
{
        virt_monitor vmonitor = new_virt_monitor();
        maxvmonitor++;
        active_monitor=maxvmonitor;
        vmonitors[maxvmonitor] = vmonitor;
        memset(vmonitor_names + 81 * active_monitor, '=', 74);
        memcpy(vmonitor_names + 81 * active_monitor + 74, " ETIOS", 6);
        memcpy(vmonitor_names + 81 * active_monitor + 2, name, strlen(name));
        *(vmonitor_names + 81 * active_monitor + 1) = ' ';
        *(vmonitor_names + 81 * active_monitor + 1 + strlen(name) + 1) = ' ';
}

//FIXME: PM should provide the right virtual monitor
virt_monitor* get_active_virt_monitor(){
        return &(vmonitors[active_monitor]);
}

char* get_active_virt_monitor_name(){
        return &(vmonitor_names[active_monitor*81]);
}
