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

/**
 * Creates a new virtual output.
 * 
 * @param *vm pointer to the virtual monitor struct, where the data should be written to
 * @param pid PID of the appropriate process
 */
void new_virt_monitor(virt_monitor *vm, uint32 pid)
{
        void *mon = mallocn(VIRTUAL_MONITOR_SIZE, "virtual monitor");
        ASSERT(mon != 0);
        for (uint16 *temp = mon; temp < (uint16*)mon + VMONITOR_HEIGHT; temp++) {
                *temp = 0xF00;
        }
        vm->begin = (uint16*)mon;
        vm->size = VIRTUAL_MONITOR_SIZE;
        vm->vis_begin = (uint16*)mon;
        vm->offset = 0;
        vm->scrolldown_limit = 0;
        vm->scrollup_limit = 0;
        vm->disable_refresh = FALSE;
        vm->pid = pid;
}

/**
 * Deletes a virtual output.
 * 
 * @param *vm virtual monitor to be deleted
 */
void free_virt_monitor(virt_monitor *vm)
{
        free((void*)vm->begin);
        if (active_monitor == maxvmonitor) {
                active_monitor--;
        } else {
                vmonitors[active_monitor] = vmonitors[maxvmonitor];
                strncpy(vmonitor_names + 81 * active_monitor, 
                                vmonitor_names + 81 * maxvmonitor, 81);
        }
        memset(&(vmonitors[maxvmonitor--]), 0, sizeof(virt_monitor));
        pm_get_proc(vmonitors[active_monitor].pid)->vmonitor = &vmonitors[active_monitor];
        pm_set_focus_proc(vmonitors[active_monitor].pid);
}

/**
 *  Writes a colored character to the virtual monitor.
 *
 * @param *vm virtual monitor, on which the character shall be written
 * @param ch character to be written
 * @param fg foreground-color
 * @param bg background color
 */
void virt_monitor_cputc(virt_monitor *vm, char ch, uint8 fg, uint8 bg)
{
        uint32 i = 0;
        uint32 temp;

        if (vm->offset >= (VMONITOR_HEIGHT / 2 - 1)) {
                if (vm->scrollup_limit < ((vm->size - VMONITOR_HEIGHT) / 0xA0)) vm->scrollup_limit++;
                vm->vis_begin += LINE_WIDTH;
                if (vm->vis_begin >= vm->begin + vm->size / 2) {
                        vm->vis_begin = vm->begin;
                }
                //scroll, if outside of display-memory
                uint16 *ptr = vm->begin +
                (vm->vis_begin + (VMONITOR_HEIGHT / 2) - LINE_WIDTH - vm->begin)
                % (vm->size / 2),
                *tmpptr;
                for (tmpptr = ptr; tmpptr < ptr + LINE_WIDTH; tmpptr++)
                        *tmpptr = 0xF00;
                vm->offset -= LINE_WIDTH;
        }
        switch (ch) {
        //        case '\a':
        //                virt_monitor_invert(get_active_virt_monitor());
        //                update_virt_monitor(get_active_virt_monitor());
        //                sleep_ticks(15); // FIXME: RACE CONDITION. Disable sleep altogether.
        //                virt_monitor_invert(get_active_virt_monitor());
        //                break;
        case '\n':
                temp = LINE_WIDTH - (vm->offset % LINE_WIDTH);
                while (i++ < temp) { //calculating the "new line" starting position
                        virt_monitor_cputc(vm, ' ', fg, bg);
                }
                break;
        case '\t':
                temp = 8 - (vm->offset % 8);
                while (i++ < temp) { //calculating the "next tab" starting position
                        virt_monitor_cputc(vm, ' ', fg, bg);
                }
                break;
        case '\b':
                if (vm->offset) {
                        vm->offset--;
                        *(vm->begin + (vm->vis_begin - vm->begin + vm->offset) % (vm->size / 2))
                        = bg * 0x1000 + fg * 0x100 + ' ';
                }
                break;
        default:
                *(vm->begin + (vm->vis_begin - vm->begin + vm->offset) % (vm->size / 2)) =
                        bg * 0x1000 + fg * 0x100 + (ch & 0xFF); //print character to the display pointer
                vm->offset++;
        }
}

/**
 * Decodes the color tag on the beginning of the given string as a color
 * 
 * @param str string starting with a tag
 * @return the decoded
 */
uint8 get_color_tag(char *str)
{
        char buf[6];
        buf[5] = '\0';
        memcpy(buf, str, 5);
        if(strcmp(buf, "{BLA}") == 0){
                return BLACK;
        }
        if(strcmp(buf, "{BLU}") == 0){
                return BLUE;
        }
        if(strcmp(buf, "{GRE}") == 0){
                return GREEN;
        }
        if(strcmp(buf, "{CYA}") == 0){
                return CYAN;
        }
        if(strcmp(buf, "{RED}") == 0){
                return RED;
        }
        if(strcmp(buf, "{VIO}") == 0){
                return VIOLET;
        }
        if(strcmp(buf, "{ORA}") == 0){
                return ORANGE;
        }
        if(strcmp(buf, "{DAR}") == 0){
                return DARKGREY;
        }
        if(strcmp(buf, "{LIG}") == 0){
                return LIGHTGREY;
        }
        if(strcmp(buf, "{LBL}") == 0){
                return LIGHTBLUE;
        }
        if(strcmp(buf, "{LGR}") == 0){
                return LIGHTGREEN;
        }
        if(strcmp(buf, "{TUR}") == 0){
                return TURQUOISE;
        }
        if(strcmp(buf, "{PIN}") == 0){
                return PINK;
        }
        if(strcmp(buf, "{MAG}") == 0){
                return MAGENTA;
        }
        if(strcmp(buf, "{YEL}") == 0){
                return YELLOW;
        }
        return WHITE;
}

/**
 *  Writes a colored null-terminated string to the virtual monitor.
 *
 * @param *vm virtual monitor, on which the string shall be written
 * @param *str pointer to the string
 * @param fg foreground-color
 * @param bg background color
 */
int virt_monitor_cputs(virt_monitor *vm, char *str, uint8 fg, uint8 bg)
{
        int tempcolor = fg;
        int res = 0;
        while (*str != 0) {
                if(*str == '#') {
                        tempcolor = get_color_tag(++str);
                        if(tempcolor != WHITE) {
                                str+=5;
                                fg = tempcolor;
                        } else if (*str != '#'){
                                virt_monitor_cputc(vm, '#', fg, bg);
                        } else {
                                str++;
                                fg = tempcolor;
                        }
                        if(*str == '\0') {
                                break;
                        }
                }
                virt_monitor_cputc(vm, *str++, fg, bg);
                res++;
        }
        return res;
}

/**
 * Writes a character to the virtual monitor.
 *
 * @param *vm virtual monitor, on which the character shall be written
 * @param ch character to be written
 */
void virt_monitor_putc(virt_monitor *vm, char ch)
{
        virt_monitor_cputc(vm, ch, WHITE, BLACK);
}

/**
 * Writes a null-terminated string to the virtual monitor.
 *
 * @param *vm virtual monitor, on which the string shall be written
 * @param *str pointer to the string
 */
int virt_monitor_puts(virt_monitor *vm, char *str)
{
        return virt_monitor_cputs(vm, str, WHITE, BLACK);
}

/**
 * Moves the cursor in the given direction.
 *
 * @param *vm virtual monitor, on which the cursur shall be moved
 * @param dir 0=UP, 1=DOWN, 2=LEFT, 3=RIGHT
 */
void virt_cursor_move(virt_monitor *vm, uint8 dir)
{
        if (vm->disable_refresh)
                return;

        switch (dir) {
        case 0:
                vm->offset =
                        (vm->offset + (VMONITOR_HEIGHT / 2) - LINE_WIDTH) % (VMONITOR_HEIGHT / 2);
                break;
        case 1:
                vm->offset = (vm->offset + LINE_WIDTH) % (VMONITOR_HEIGHT / 2);
                break;
        case 2:
                vm->offset = (vm->offset - vm->offset % LINE_WIDTH) +
                (vm->offset % LINE_WIDTH + 80 - 1) % LINE_WIDTH;
                break;
        case 3:
                vm->offset =
                        (vm->offset - vm->offset % LINE_WIDTH) + (vm->offset + 1) % LINE_WIDTH;
                break;
        }
        //Cursor update
        outb(0x3D4, 0x0E);
        outb(0x3D5, (vm->offset + CURSOR_OFFSET) >> 8);
        outb(0x3D4, 0x0F);
        outb(0x3D5, (vm->offset + CURSOR_OFFSET));
}

/**
 * Updates the virtual monitor.
 *
 * @param *vm virtual monitor to be updated
 */
void update_virt_monitor(virt_monitor *vm)
{
        if (vm->disable_refresh) {
                //disable cursor
                outb(0x3D4, 0x0E);
                outb(0x3D5, 0x20);
                return;
        }

        set_disp(VGA_DISPLAY);
        monitor_cputs(get_active_virt_monitor_name(), GREEN, BLACK);

        if ((vm->begin + vm->size / 2) - vm->vis_begin >= VMONITOR_HEIGHT) {
                memcpy((void*)MONITOR_START, (void*)(vm->vis_begin), VMONITOR_HEIGHT);
        } else {
                memcpy((void*)MONITOR_START, (void*)(vm->vis_begin),
                                2*(vm->begin + vm->size / 2 - vm->vis_begin));
                memcpy((void*)MONITOR_START + 2*(vm->begin + vm->size / 2 - vm->vis_begin),
                                (void*)(vm->begin),
                                2*(VMONITOR_HEIGHT - (vm->begin + vm->size / 2 - vm->vis_begin)));
        }
        //Cursor update
        outb(0x3D4, 0x0E);
        outb(0x3D5, (vm->offset + CURSOR_OFFSET) >> 8);
        outb(0x3D4, 0x0F);
        outb(0x3D5, (vm->offset + CURSOR_OFFSET));
}

/**
 * Scrolls a virtual monitor up on request.
 *
 * @param *vm virtual monitor to scroll
 */
void virt_monitor_scrollup(virt_monitor *vm)
{
        if (vm->scrollup_limit) {
                vm->scrollup_limit--;
                if (vm->vis_begin > vm->begin) vm->vis_begin -= LINE_WIDTH;
                else vm->vis_begin = vm->begin + (vm->size / 2) - LINE_WIDTH;
                vm->scrolldown_limit++;
        }
}

/**
 * Scrolls a virtual monitor down on request.
 *
 * @param *vm virtual monitor to scroll
 */
void virt_monitor_scrolldown(virt_monitor *vm)
{
        if (vm->scrolldown_limit) {
                vm->scrolldown_limit--;
                if (vm->vis_begin < vm->begin + (vm->size / 2)) vm->vis_begin += LINE_WIDTH;
                else vm->vis_begin = vm->begin;
                vm->scrollup_limit++;
        }
}

/**
 * Inverts a virtual monitor.
 * 
 * @param *vm virtual monitor to invert
 */
void virt_monitor_invert(virt_monitor *vm)
{
        for (uint8 *temp = (uint8*)vm->vis_begin + 1;
        temp <= (uint8*)(vm->vis_begin + VMONITOR_HEIGHT); temp += 2)
                * temp = 0xFF - *temp;
}
