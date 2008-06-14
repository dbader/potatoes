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
 * Header for the virtual monitor structure
 * 
 * @author Dmitriy Traytel
 * @author $LastChangedBy: dtraytel $
 * @version $Rev: 119 $
 */

#ifndef __IO_VIRTUAL_H_
#define __IO_VIRTUAL_H_

#include "../include/types.h"

#define VIRTUAL_MONITOR_SIZE 160000

typedef struct{
        uint16 *begin;
        uint32 size;
        uint16 *vis_begin;
        uint32 charnum;
        uint32 scrolldown_limit;
        uint32 scrollup_limit;
}virt_monitor;

virt_monitor* get_active_virt_monitor();

virt_monitor new_virt_monitor();
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
void virt_monitor_cputs(virt_monitor *vm, char *str, uint8 fg, uint8 bg);
//prints black character
void virt_monitor_putc(virt_monitor *vm, char ch);
//prints black string
void virt_monitor_puts(virt_monitor *vm, char *str);

void start_vmonitor();
void start_shell();
#endif /*__IO_VIRTUAL_H_*/
