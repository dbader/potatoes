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
(at your option) any later version->

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE->  See the
GNU General Public License for more details->

You should have received a copy of the GNU General Public License
along with this program->  If not, see <http://www->gnu->org/licenses/>->
 */

/**
 * @file
 * Function used by virtual outputs.
 *
 * @author Dmitriy Traytel
 * @author $LastChangedBy$
 * @version $Rev$
 */
#include "../include/stdlib.h"
#include "../include/stdio.h"
#include "../include/string.h"
#include "../include/util.h"
#include "../include/debug.h"
#include "../include/assert.h"
#include "../include/const.h"
#include "../pm/pm_main.h"

#include "io.h"
#include "io_rtc.h"
#include "io_virtual.h"

const uint8 line_width = 80;
const uint16 vmonitor_height = 0xF00;
const uint32 monitor_start = 0xB80A0;
const uint8 cursor_offset = 80;


/**
 * Creates a new virtual output.
 */
virt_monitor new_virt_monitor(uint32 pid)
{
        virt_monitor *mon_ptr;
        ASSERT(mon_ptr != 0);
        void *mon = mallocn(VIRTUAL_MONITOR_SIZE, "virtual monitor");
        ASSERT(mon != 0);
        for (uint16 *temp = mon; temp < (uint16*)mon + vmonitor_height; temp++) {
                *temp = 0xF00;
        }
        mon_ptr->begin = (uint16*)mon;
        mon_ptr->size = VIRTUAL_MONITOR_SIZE;
        mon_ptr->vis_begin = (uint16*)mon;
        mon_ptr->charnum = 0;
        mon_ptr->scrolldown_limit = 0;
        mon_ptr->scrollup_limit = 0;
        mon_ptr->disable_refresh = FALSE;
        mon_ptr->pid = pid;
        return *mon_ptr;
}

/**
 * Deletes a virtual output.
 */
void free_virt_monitor(virt_monitor *vm)
{
        free((void*)vm->begin);
        if (active_monitor == maxvmonitor) {
                active_monitor--;
        } else {
                vmonitors[active_monitor] = vmonitors[maxvmonitor];
        }
        memset(&(vmonitors[maxvmonitor--]), 0, sizeof(virt_monitor));
        
       // pm_set_focus_proc(vmonitors[active_monitor].pid);
}

/**
 *  Writes a colored character to the virtual monitor.
 *
 * @param *vm active virtual monitor
 * @param ch character to be written
 * @param fg foreground-color
 * @param bg background color
 */
void virt_monitor_cputc(virt_monitor *vm, char ch, uint8 fg, uint8 bg)
{
        uint32 i = 0;
        uint32 temp;

        if (vm->charnum >= (vmonitor_height / 2 - 1)) {
                if (vm->scrollup_limit < ((vm->size - vmonitor_height) / 0xA0)) vm->scrollup_limit++;
                vm->vis_begin += line_width;
                if (vm->vis_begin >= vm->begin + vm->size / 2) {
                        vm->vis_begin = vm->begin;
                }
                //scroll, if outside of display-memory
                uint16 *ptr = vm->begin +
                              (vm->vis_begin + (vmonitor_height / 2) - line_width - vm->begin)
                              % (vm->size / 2),
                              *tmpptr;
                for (tmpptr = ptr; tmpptr < ptr + line_width; tmpptr++)
                        *tmpptr = 0xF00;
                vm->charnum -= line_width;
        }
        switch (ch) {
//        case '\a':
//                virt_monitor_invert(get_active_virt_monitor());
//                update_virt_monitor(get_active_virt_monitor());
//                sleep_ticks(15); // FIXME: RACE CONDITION. Disable sleep altogether.
//                virt_monitor_invert(get_active_virt_monitor());
//                break;
        case '\n':
                temp = line_width - (vm->charnum % line_width);
                while (i++ < temp) { //calculating the "new line" starting position
                        virt_monitor_cputc(vm, ' ', fg, bg);
                }
                break;
        case '\t':
                temp = 8 - (vm->charnum % 8);
                while (i++ < temp) { //calculating the "next tab" starting position
                        virt_monitor_cputc(vm, ' ', fg, bg);
                }
                break;
        case '\b':
                if (vm->charnum) {
                        vm->charnum--;
                        *(vm->begin + (vm->vis_begin - vm->begin + vm->charnum) % (vm->size / 2))
                        = bg * 0x1000 + fg * 0x100 + ' ';
                }
                break;
        default:
                *(vm->begin + (vm->vis_begin - vm->begin + vm->charnum) % (vm->size / 2)) =
                        bg * 0x1000 + fg * 0x100 + (ch & 0xFF); //print character to the display pointer
                vm->charnum++;
        }
}

/**
 *  Writes a colored null-terminated string to the virtual monitor.
 *
 * @param *vm active virtual monitor
 * @param *str pointer to the string
 * @param fg foreground-color
 * @param bg background color
 */
int virt_monitor_cputs(virt_monitor *vm, char *str, uint8 fg, uint8 bg)
{
        int res = 0;
        while (*str != 0) {
                virt_monitor_cputc(vm, *str, fg, bg);
                str += 1;
                res++;
        }
        return res;
}

/**
 * Writes a character to the virtual monitor.
 *
 * @param *vm active virtual monitor
 * @param ch character to be written
 */
void virt_monitor_putc(virt_monitor *vm, char ch)
{
        virt_monitor_cputc(vm, ch, WHITE, BLACK);
}

/**
 * Writes a null-terminated string to the virtual monitor.
 *
 * @param *vm active virtual monitor
 * @param *str pointer to the string
 */
int virt_monitor_puts(virt_monitor *vm, char *str)
{
        return virt_monitor_cputs(vm, str, WHITE, BLACK);
}

/**
 * Moves the cursor in the given direction.
 *
 * @param *vm active virtual monitor
 * @param dir 0=UP, 1=DOWN, 2=LEFT, 3=RIGHT
 */
void virt_cursor_move(virt_monitor *vm, uint8 dir)
{
        if (vm->disable_refresh)
                return;

        switch (dir) {
        case 0:
                vm->charnum =
                        (vm->charnum + (vmonitor_height / 2) - line_width) % (vmonitor_height / 2);
                break;
        case 1:
                vm->charnum = (vm->charnum + line_width) % (vmonitor_height / 2);
                break;
        case 2:
                vm->charnum = (vm->charnum - vm->charnum % line_width) +
                              (vm->charnum % line_width + 80 - 1) % line_width;
                break;
        case 3:
                vm->charnum =
                        (vm->charnum - vm->charnum % line_width) + (vm->charnum + 1) % line_width;
                break;
        }
        //Cursor update
        outb(0x3D4, 0x0E);
        outb(0x3D5, (vm->charnum + cursor_offset) >> 8);
        outb(0x3D4, 0x0F);
        outb(0x3D5, (vm->charnum + cursor_offset));
}

/**
 * Updates the virtual monitor.
 *
 * @param *vm active virtual monitor
 */
void update_virt_monitor(virt_monitor *vm)
{
        if (vm->disable_refresh) {
                //disable cursor
                outb(0x3D4, 0x0E);
                outb(0x3D5, 0x20);
                return;
        }

        set_disp(0xB8000);
        monitor_cputs(get_active_virt_monitor_name(), GREEN, BLACK);

        if ((vm->begin + vm->size / 2) - vm->vis_begin >= vmonitor_height) {
                memcpy((void*)monitor_start, (void*)(vm->vis_begin), vmonitor_height);
        } else {
                memcpy((void*)monitor_start, (void*)(vm->vis_begin),
                       2*(vm->begin + vm->size / 2 - vm->vis_begin));
                memcpy((void*)monitor_start + 2*(vm->begin + vm->size / 2 - vm->vis_begin),
                       (void*)(vm->begin),
                       2*(vmonitor_height - (vm->begin + vm->size / 2 - vm->vis_begin)));
        }
        //Cursor update
        outb(0x3D4, 0x0E);
        outb(0x3D5, (vm->charnum + cursor_offset) >> 8);
        outb(0x3D4, 0x0F);
        outb(0x3D5, (vm->charnum + cursor_offset));
}

/**
 * Scrolls the virtual monitor up on request.
 *
 * @param *vm active virtual monitor
 */
void virt_monitor_scrollup(virt_monitor *vm)
{
        if (vm->scrollup_limit) {
                vm->scrollup_limit--;
                if (vm->vis_begin > vm->begin) vm->vis_begin -= line_width;
                else vm->vis_begin = vm->begin + (vm->size / 2) - line_width;
                vm->scrolldown_limit++;
        }
}

/**
 * Scrolls the virtual monitor down on request.
 *
 * @param *vm active virtual monitor
 */
void virt_monitor_scrolldown(virt_monitor *vm)
{
        if (vm->scrolldown_limit) {
                vm->scrolldown_limit--;
                if (vm->vis_begin < vm->begin + (vm->size / 2)) vm->vis_begin += line_width;
                else vm->vis_begin = vm->begin;
                vm->scrollup_limit++;
        }
}

void virt_monitor_invert(virt_monitor *vm)
{
        for (uint8 *temp = (uint8*)vm->vis_begin + 1;
                        temp <= (uint8*)(vm->vis_begin + vmonitor_height); temp += 2)
                * temp = 0xFF - *temp;
}
