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
 * Handler for hardware-interrupts.
 *
 * @author Dmitriy Traytel
 * @author $LastChangedBy$
 * @version $Rev$
 */

#include "../include/types.h"
#include "../include/stdio.h"

#include "../io/int_handler.h"
#include "../pm/pm_main.h"

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();
extern void idt_fill_entry(uint8 pos, uint32 offset, uint16 sel, uint8 flg);

extern void outb(uint8 port, uint8 value);

/**
 * Remaps the master and slave programable interrupt controller to the idt-entries 32-47
 */
void pic_remap()
{
        outb(0x20, 0x11);
        outb(0xA0, 0x11);
        outb(0x21, 0x20); //IDT #32
        outb(0xA1, 0x28); //IDT #40
        outb(0x21, 0x04);
        outb(0xA1, 0x02);
        outb(0x21, 0x01);
        outb(0xA1, 0x01);
        outb(0x21, 0x0);
        outb(0xA1, 0x0);
}

/**
 * Inits IRQ-support
 */
void irq_init()
{
        pic_remap();
        idt_fill_entry(32, (uint32)irq0, 0x8, 0x8E);
        idt_fill_entry(33, (uint32)irq1, 0x8, 0x8E);
        idt_fill_entry(34, (uint32)irq2, 0x8, 0x8E);
        idt_fill_entry(35, (uint32)irq3, 0x8, 0x8E);
        idt_fill_entry(36, (uint32)irq4, 0x8, 0x8E);
        idt_fill_entry(37, (uint32)irq5, 0x8, 0x8E);
        idt_fill_entry(38, (uint32)irq6, 0x8, 0x8E);
        idt_fill_entry(39, (uint32)irq7, 0x8, 0x8E);
        idt_fill_entry(40, (uint32)irq8, 0x8, 0x8E);
        idt_fill_entry(41, (uint32)irq9, 0x8, 0x8E);
        idt_fill_entry(42, (uint32)irq10, 0x8, 0x8E);
        idt_fill_entry(43, (uint32)irq11, 0x8, 0x8E);
        idt_fill_entry(44, (uint32)irq12, 0x8, 0x8E);
        idt_fill_entry(45, (uint32)irq13, 0x8, 0x8E);
        idt_fill_entry(46, (uint32)irq14, 0x8, 0x8E);
        idt_fill_entry(47, (uint32)irq15, 0x8, 0x8E);
}

char *hw_messages[] = {
        "timer",
        "keyboard",
        "irq 9",
        "com 2,4,6,8",
        "com 1,3,5,7",
        "ltp 2",
        "floppy",
        "ltp 1",
        "rtc",
        "vga",
        "pci",
        "scsi",
        "ps/2",
        "coprocessor",
        "primary ide",
        "secondary ide"
};

/**
 * The programable interrupt controller needs to be reactivated after every interrupt. 
 * This procedure provides this.
 *
 * @param slave should the slave pic be remapped too? (0 if not)
 */
void reactivate_pic(bool slave)
{
        if (slave)
                outb(0xA0, 0x20);
        outb(0x20, 0x20);
}

/**
 * Distributes the interrupts to their handlers.
 *
 * @param int_no the hardware interrupt number
 * @param context a snapshot of the cpu registers at the time of the interrupt.
 * @return the context of the new active process (after scheduling)
 */
uint32 irq_handler(uint32 int_no, uint32 context)
{
        uint32 ret = context;

        switch (int_no - 32) {
        case 0:
                ret = timer_handler(context);
                break;
        case 1:
                kb_handler();
                break;
        case 2:
                break;
        case 3:
                break;
        case 4:
                break;
        case 5:
                break;
        case 6:
                break;
        case 7:
                break;
        case 8:
                break;
        case 9:
                break;
        case 10:
                break;
        case 11:
                break;
        case 12:
                break;
        case 13:
                break;
        case 14:
                hd_handler();
                break;
        case 15:
                break;
        }

        if (int_no > 40)
                reactivate_pic(1);
        else
                reactivate_pic(0);

        return ret;
}
