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

#include "io.h"
#include "io_virtual.h"

/**
 * Creates a new virtual output.
 */
virt_monitor new_virt_monitor(){
        virt_monitor *mon_ptr = 
                (virt_monitor*)mallocn(sizeof(virt_monitor),"virtual monitor pointer");
        ASSERT(mon_ptr!=0);
        void *mon = mallocn(VIRTUAL_MONITOR_SIZE,"virtual monitor");
        ASSERT(mon!=0);
        for(uint16 *temp = mon; temp < (uint16*)mon + 0xFA0; temp++){
                        *temp = 0xF00;
        }
        mon_ptr->begin = (uint16*)mon;
        mon_ptr->size = VIRTUAL_MONITOR_SIZE;
        mon_ptr->vis_begin = (uint16*)mon;
        mon_ptr->charnum = 0;
        mon_ptr->scrolldown_limit = 0;
        mon_ptr->scrollup_limit = 0;
        return *mon_ptr;
}

/**
 * Deletes a virtual output.
 */
void free_virt_monitor(virt_monitor *vm)
{
        free((void*)vm->begin);
        free(&vm);
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
        uint32 i=0;
        uint32 temp;
        
        if(vm->charnum >= 1999){
                if(vm->scrollup_limit < (vm->size / 0xA0) - 25) vm->scrollup_limit++;
                vm->vis_begin += 80; 
                if (vm->vis_begin >= vm->begin + vm->size / 2){
                        vm->vis_begin = vm->begin;                        
                }
                //scroll, if outside of display-memory
                uint16 *ptr = vm->begin + (vm->vis_begin + (0xF00 / 2) - vm->begin) % (vm->size / 2),
                       *tmpptr;
                for (tmpptr = ptr; tmpptr < ptr + 80; tmpptr++)
                       *tmpptr = 0xF00;
                vm->charnum -= 80;
        }
        switch(ch){
        case '\a':
                virt_monitor_invert(get_active_virt_monitor());
                update_virt_monitor(get_active_virt_monitor());
                sleep_ticks(15);
                virt_monitor_invert(get_active_virt_monitor());
                break;
        case '\n':
                temp= 80 - (vm->charnum % 80);
                while(i++ < temp){ //calculating the "new line" starting position
                        virt_monitor_cputc(vm, ' ',fg,bg);
                }
                break;
        case '\t':
                temp = 8 - (vm->charnum % 8);
                while(i++ < temp){ //calculating the "next tab" starting position
                        virt_monitor_cputc(vm, ' ',fg,bg);
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
                        bg * 0x1000 + fg * 0x100 + ch; //print character to the display pointer
                vm->charnum++;
        }
        //Cursor update
        outb(0x3D4, 0x0E);
        outb(0x3D5, (vm->charnum) >> 8);
        outb(0x3D4, 0x0F);
        outb(0x3D5, (vm->charnum));
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
        while(*str!=0){
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
        switch (dir){
        case 0: 
                vm->charnum = (vm->charnum + 2000 - 80) % 2000; 
                break;
        case 1: 
                vm->charnum = (vm->charnum + 80) % 2000; 
                break;
        case 2: 
                vm->charnum = (vm->charnum - vm->charnum % 80) + (vm->charnum % 80 +80 - 1) % 80;
                break;
        case 3: 
                vm->charnum = (vm->charnum - vm->charnum % 80) + (vm->charnum +1) % 80;
                break;
        }
        //Cursor update
        outb(0x3D4, 0x0E);
        outb(0x3D5, (vm->charnum) >> 8);
        outb(0x3D4, 0x0F);
        outb(0x3D5, (vm->charnum));
}

/**
 * Updates the virtual monitor. 
 * 
 * @param *vm active virtual monitor
 */
void update_virt_monitor(virt_monitor *vm)
{
        if((vm->begin + vm->size / 2) - vm->vis_begin >= 0xFA0){
                memcpy((void*)0xB8000, (void*)(vm->vis_begin), 0xFA0);
        }
        else{
                memcpy((void*)0xB8000, (void*)(vm->vis_begin), 
                                2*(vm->begin + vm->size / 2 - vm->vis_begin));
                memcpy((void*)0xB8000 + 2*(vm->begin + vm->size / 2 - vm->vis_begin),
                                (void*)(vm->begin),
                                2*(0xFA0 - (vm->begin + vm->size / 2 - vm->vis_begin)));
        }
}

/**
 * Scrolls the virtual monitor up on request.
 * 
 * @param *vm active virtual monitor
 */
void virt_monitor_scrollup(virt_monitor *vm)
{
        if(vm->scrollup_limit){
                vm->scrollup_limit--;
                if(vm->vis_begin > vm->begin) vm->vis_begin -= 80;
                else vm->vis_begin = vm->begin + (vm->size / 2) - 80;
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
        if(vm->scrolldown_limit){
                vm->scrolldown_limit--;
                if(vm->vis_begin < vm->begin + (vm->size / 2)) vm->vis_begin += 80;
                else vm->vis_begin = vm->begin;
                vm->scrollup_limit++;
        }
}

void virt_monitor_invert(virt_monitor *vm)
{
        for(uint8 *temp = (uint8*)vm->vis_begin + 1;
                        temp <= (uint8*)(vm->vis_begin + 80 * 25); temp+=2)
                *temp = 0xFF - *temp;
}