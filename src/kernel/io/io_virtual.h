/* $Id: io_virtual.h 266 2009-10-14 08:15:36Z dtraytel $
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
 * Header for the virtual monitor structure
 *
 * @author Dmitriy Traytel
 * @author $LastChangedBy: dtraytel $
 * @version $Rev: 266 $
 */

#ifndef __IO_VIRTUAL_H_
#define __IO_VIRTUAL_H_

#include "../include/types.h"

#define VIRTUAL_MONITOR_SIZE 160000
#define LINE_WIDTH 80
#define VMONITOR_HEIGHT 0xF00
#define MONITOR_START 0xB80A0
#define CURSOR_OFFSET 80

extern uint16 maxvmonitor;
extern uint16 active_monitor;

/**
 * Structure that represents a virtual monitor
 */
typedef struct {
        /**
         * Pointer to the start of the allocated memory
         */
        uint16 *begin;

        /**
         * VMonitor name
         */
        char *name;

        /**
         * Size of the allocated memory
         */
        uint32 size;

        /**
         * Pointer to the start of the visible pane
         */
        uint16 *vis_begin;

        /**
         * Position of the cursor on the visible pane
         */
        uint32 offset;

        /**
         * Number of lines beneath the visible pane
         */
        uint32 scrolldown_limit;

        /**
         * Number of lines above the visible pane
         */
        uint32 scrollup_limit;

        /**
         * For framebuffer access. If this is false, the virtual monitor will not be painted.
         */
        bool disable_refresh;

        /**
         * The PID of the process that owns this virtual monitor
         */
        uint32 pid;
}virt_monitor;

//FIXME: vmonitors as linked list
extern virt_monitor *vmonitors;

virt_monitor* get_active_virt_monitor();
char* get_active_virt_monitor_name();

void new_virt_monitor(virt_monitor *vm, uint32 pid);
void free_virt_monitor(virt_monitor *vm);

void update_virt_monitor();

void virt_cursor_move(virt_monitor *vm, uint8 dir);
void virt_monitor_scrolldown(virt_monitor *vm);
void virt_monitor_scrollup(virt_monitor *vm);
void virt_monitor_invert(virt_monitor *vm);

void virt_printf(virt_monitor vm, char *fmt, ...);

//prints colored character
void virt_monitor_cputc(virt_monitor *vm, char ch, uint8 fg, uint8 bg);
//prints colored string
int virt_monitor_cputs(virt_monitor *vm, char *str, uint8 fg, uint8 bg);
//prints black character
void virt_monitor_putc(virt_monitor *vm, char ch);
//prints black string
int virt_monitor_puts(virt_monitor *vm, char *str);

void init_vmonitors();
virt_monitor* start_vmonitor(char *name, uint32 pid);

void switch_monitor_down();
void switch_monitor_up();
#endif /*__IO_VIRTUAL_H_*/
