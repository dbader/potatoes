/* $Id: int_idt.c 266 2009-10-14 08:15:36Z dtraytel $
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
 * Builds and initializes the interrupt descriptor table.
 *
 * @author Dmitriy Traytel
 * @author $LastChangedBy: dtraytel $
 * @version $Rev: 266 $
 */

#include "../include/types.h"

/**
 * The structure of one IDT entry.
 */
struct idt_entry{ //16+16+8+8+16=64 bit
        /**
         * Lower 16 bit of the interrupt handler's code address
         */
        uint16 low_offset;
        
        /**
         * Code segment selector in the GDT.
         */
        uint16 selector;
        
        /**
         * Unused
         */
        uint8 separator; //always 0
        
        /***********************************************
         * 0_____1___________3_______________________7 *
         * |  P  |    DPL    |         01110         | *
         * |_____|___________|_______________________| *
         * P=segment present?                          *
         * DPL=descriptor priveledge level             *
         ***********************************************/
        uint8 flags;
        
        /**
         * Upper 16 bit of the interrupt handler's code address
         */
        uint16 high_offset;
}__attribute__((packed)); //gcc-flag to use 64 connected bits of memory for the struct

/**
 * The structure of the IDT pointer which tells the processor where to find our IDT
 */
struct idt_pointer { //16+32=48 bit
        uint16 maxsize;
        uint32 start;
}__attribute__((packed)); //gcc-flag to use 64 connected bits of memory for the struct

/**
 * Our idt with 256 descriptors
 */
struct idt_entry idt[256];

/**
 * The pointer to our idt
 */
struct idt_pointer idtp;

/**
 * Makes a new idt-entry
 *
 * @param pos Number of the new idt-entry
 * @param offset Offset to be added to the base address
 * @param sel Selector for the segment's base address
 * @param flg Deskriptor flags
 */
void idt_fill_entry(uint8 pos, uint32 offset, uint16 sel, uint8 flg)
{
        idt[pos].selector = sel;
        idt[pos].flags = flg;
        idt[pos].separator = 0;
        idt[pos].low_offset = offset & 0xFFFF;
        idt[pos].high_offset = offset >> 16;
}

/**
 * Initializes a new idt with blank entries
 */
extern void idt_flush(uint32);
void idt_init()
{
        idtp.maxsize = (sizeof(struct idt_entry) * 256) - 1;
        idtp.start = (uint32)&idt;
        int i;
        for (i = 0; i < 256; i++) {             //fill with empty descriptors
                idt_fill_entry(i, 0, 0, 0);
        }
        idt_flush((uint32)&idtp);
}
