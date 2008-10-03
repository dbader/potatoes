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
#include "../include/assert.h"
#include "../pm/pm_main.h"

#include "io_rtc.h"
#include "io_virtual.h"

uint16 active_monitor = -1;
uint16 maxvmonitor = -1;
virt_monitor *vmonitors;
uint16 num_vmonitor_limit = 1000;
char *vmonitor_names;

void switch_monitor_up()
{
        if (active_monitor < maxvmonitor) active_monitor++;
        else if (active_monitor == maxvmonitor) active_monitor = 0;
        
//        pm_set_focus_proc(vmonitors[active_monitor].pid);
}

void switch_monitor_down()
{
        if (active_monitor > 0) active_monitor--;
        else if (active_monitor == 0) active_monitor = maxvmonitor;
        
//        pm_set_focus_proc(vmonitors[active_monitor].pid);
}

void init_vmonitors()
{
        rtc_init();
        vmonitor_names =
                (char*) callocn(num_vmonitor_limit * 81, sizeof(char), "vmonitor names array");
        ASSERT(vmonitor_names != 0);
        vmonitors =
                (virt_monitor*)callocn(num_vmonitor_limit, sizeof(virt_monitor*), "vmonitors array");
        ASSERT(vmonitors != 0);
        start_vmonitor("DEBUG MONITOR", 0);
}

/**
 * Initializes the virtual monitor.
 */
virt_monitor* start_vmonitor(char *name, uint32 pid)
{
        maxvmonitor++;
        active_monitor = maxvmonitor;
        vmonitors[maxvmonitor] = new_virt_monitor(pid);
        memset(vmonitor_names + 81 * active_monitor, '=', 80);
        memcpy(vmonitor_names + 81 * active_monitor + 2, name, strlen(name));
        *(vmonitor_names + 81 * active_monitor + 1) = ' ';
        *(vmonitor_names + 81 * active_monitor + 1 + strlen(name) + 1) = ' ';
        *(vmonitor_names + 81 * active_monitor + 54) = ' ';
        *(vmonitor_names + 81 * active_monitor + 78) = ' ';
        return &(vmonitors[maxvmonitor]);
}

virt_monitor* get_active_virt_monitor()
{
        return &(vmonitors[active_monitor]);
}

char* get_active_virt_monitor_name()
{
        memcpy(vmonitor_names + 81 * active_monitor + 55, (void*)time2str(), 23);
        return &(vmonitor_names[active_monitor*81]);
}
