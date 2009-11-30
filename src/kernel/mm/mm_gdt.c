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
 * Creates and initializes the global descriptor table.
 *
 * based on JamesM's kernel development tutorials
 *
 * @author Johannes Schamburger
 * @author $LastChangedBy$
 * @version $Rev$
 */

#include "../include/types.h"
#include "../include/debug.h"
#include "mm.h"

extern void gdt_flush();

/**
 * defines one GDT entry
 * @param num           number of the entry that shall be added
 * @param base          base address
 * @param limit         end address
 * @param access        access byte (structure @see mm.h)
 * @param gran          granularity byte (structure @see mm.h) 
 */
void gdt_add_entry(sint32 num, uint32 base, uint32 limit, uint8 access, uint8 gran)
{
        dprintf("#{LBL}mm:## GDT[%d] 0x%x - 0x%x access: 0x%x gran: 0x%x\n",
                num, base, limit, access, gran);
        
        // split the base
        gdt[num].base_low = (base & 0xFFFF);
        gdt[num].base_middle = (base >> 16) & 0xFF;
        gdt[num].base_high = (base >> 24) & 0xFF; 

        gdt[num].limit_low = (limit & 0xFFFF);          // get the lower 16 bits of the limit
        gdt[num].granularity = (limit >> 16) & 0x0F;    /* set the last 4 bits of the granularity 
                                                           byte to the bits 19 - 16 of the limit */

        gdt[num].granularity |= gran & 0xF0;            /* unset the last 4 bits of gran (which are 
                                                           already set above) and set the upper 4 bits 
                                                           of gdt[num].granularity according to gran */  
        gdt[num].access = access;
}

/**
 * initializes the GDT
 */
void gdt_init()
{
        gp.limit = (sizeof(struct gdt_entry) * 3) - 1; // the size of the GDT
        gp.base = (uint32) & gdt; // the base address of the GDT

        gdt_add_entry(0, 0, 0, 0, 0);                // first entry: the null descriptor
        gdt_add_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // second entry: code segment
        gdt_add_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // third entry: data segment

        gdt_flush((uint32)&gp); // replace the old GDT by the new one
}
