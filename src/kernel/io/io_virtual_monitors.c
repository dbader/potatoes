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

/**
 * Switches to the next virtual monitor.
 */
void switch_monitor_up()
{
        if (active_monitor < maxvmonitor) active_monitor++;
        else if (active_monitor == maxvmonitor) active_monitor = 0;

        pm_set_focus_proc(vmonitors[active_monitor].pid);
}

/**
 * Switches to the previous virtual monitor.
 */
void switch_monitor_down()
{
        if (active_monitor > 0) active_monitor--;
        else if (active_monitor == 0) active_monitor = maxvmonitor;

        pm_set_focus_proc(vmonitors[active_monitor].pid);
}

/**
 * Initializes the virtual monitors.
 */
void init_vmonitors()
{
        rtc_init();
        vmonitors =
                (virt_monitor*)callocn(num_vmonitor_limit, sizeof(virt_monitor), "vmonitors array");
        ASSERT(vmonitors != 0);
        start_vmonitor("DEBUG MONITOR", 0);
}

/**
 * Initializes and registers a virtual monitor.
 *
 * @param *name name of the new virtual monitor
 * @param pid PID of the appropriate process
 * @return pointer to the new virtual monitor
 */
virt_monitor* start_vmonitor(char *name, uint32 pid)
{
        maxvmonitor++;
        new_virt_monitor(&vmonitors[maxvmonitor], pid);
        active_monitor = maxvmonitor;
        memset(vmonitors[active_monitor].name, '=', 80);
        memcpy(vmonitors[active_monitor].name + 2, name, strlen(name));
        *(vmonitors[active_monitor].name + 1) = ' ';
        *(vmonitors[active_monitor].name + 1 + strlen(name) + 1) = ' ';
        *(vmonitors[active_monitor].name + 54) = ' ';
        *(vmonitors[active_monitor].name + 78) = ' ';
        return &(vmonitors[maxvmonitor]);
}

/**
 * Returns the pointer to the active virtual monitor
 *
 * @return pointer to the active virtual monitor
 */
virt_monitor* get_active_virt_monitor()
{
        return &(vmonitors[active_monitor]);
}

/**
 * Returns the name of the active virtual monitor
 *
 * @return pointer to the name string
 */
char* get_active_virt_monitor_name()
{
        static char timestamp[24];
        memcpy(vmonitors[active_monitor].name + 55, time2str(time, timestamp), 23);
        return vmonitors[active_monitor].name;
}
