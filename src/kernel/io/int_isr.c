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
 * Handler for exceptions
 *
 * @author Dmitriy Traytel
 * @author $LastChangedBy$
 * @version $Rev$
 */

#include "../include/types.h"
#include "../include/stdio.h"
#include "../include/init.h"

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();
extern void idt_fill_entry(uint8 pos, uint32 offset, uint16 sel, uint8 flg);

/***********************************************************
 *       1       2       3       4       5       6       7 *
 * flags |   P   |      DPL      |         01110         | *
 * P=segment present? DPL=descriptor priveledge level      *
 ***********************************************************/

void isr_init() 
{
      idt_fill_entry(0, (uint32)isr0, 0x8, 0x8E);
      idt_fill_entry(1, (uint32)isr1, 0x8, 0x8E);
      idt_fill_entry(2, (uint32)isr2, 0x8, 0x8E);
      idt_fill_entry(3, (uint32)isr3, 0x8, 0x8E);
      idt_fill_entry(4, (uint32)isr4, 0x8, 0x8E);
      idt_fill_entry(5, (uint32)isr5, 0x8, 0x8E);
      idt_fill_entry(6, (uint32)isr6, 0x8, 0x8E);
      idt_fill_entry(7, (uint32)isr7, 0x8, 0x8E);
      idt_fill_entry(8, (uint32)isr8, 0x8, 0x8E);
      idt_fill_entry(9, (uint32)isr9, 0x8, 0x8E);
      idt_fill_entry(10, (uint32)isr10, 0x8, 0x8E);
      idt_fill_entry(11, (uint32)isr11, 0x8, 0x8E);
      idt_fill_entry(12, (uint32)isr12, 0x8, 0x8E);
      idt_fill_entry(13, (uint32)isr13, 0x8, 0x8E);
      idt_fill_entry(14, (uint32)isr14, 0x8, 0x8E);
      idt_fill_entry(15, (uint32)isr15, 0x8, 0x8E);
      idt_fill_entry(16, (uint32)isr16, 0x8, 0x8E);
      idt_fill_entry(17, (uint32)isr17, 0x8, 0x8E);
      idt_fill_entry(18, (uint32)isr18, 0x8, 0x8E);
      idt_fill_entry(19, (uint32)isr19, 0x8, 0x8E);
      idt_fill_entry(20, (uint32)isr20, 0x8, 0x8E);
      idt_fill_entry(21, (uint32)isr21, 0x8, 0x8E);
      idt_fill_entry(22, (uint32)isr22, 0x8, 0x8E);
      idt_fill_entry(23, (uint32)isr23, 0x8, 0x8E);
      idt_fill_entry(24, (uint32)isr24, 0x8, 0x8E);
      idt_fill_entry(25, (uint32)isr25, 0x8, 0x8E);
      idt_fill_entry(26, (uint32)isr26, 0x8, 0x8E);
      idt_fill_entry(27, (uint32)isr27, 0x8, 0x8E);
      idt_fill_entry(28, (uint32)isr28, 0x8, 0x8E);
      idt_fill_entry(29, (uint32)isr29, 0x8, 0x8E);
      idt_fill_entry(30, (uint32)isr30, 0x8, 0x8E);
      idt_fill_entry(31, (uint32)isr31, 0x8, 0x8E);
}

char *ex_messages[] = {
        "division by zero",
        "debug",
        "not maskable interrupt",
        "breakpoint",
        "overflow",
        "out of bounds",
        "invalid opcode",
        "no coprocessor",
        "double fault",
        "coprocessor segment overrun",
        "bad tss",
        "segment not present",
        "stack fault",
        "general protection",
        "page fault",
        "unknown interrupt",
        "coprocessor fault",
        "reserved",
        "reserved",
        "reserved",
        "reserved",       
        "reserved",
        "reserved",
        "reserved",
        "reserved",
        "reserved",
        "reserved",
        "reserved",
        "reserved",
        "reserved",
        "reserved",
        "reserved",
        "reserved"       
};

void ex_handler(uint32 num)
{
        panic(ex_messages[num]);
}
