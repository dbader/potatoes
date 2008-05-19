#ifndef __IO_H_
#define __IO_H_

#include "../include/types.h"

void idt_init();
void isr_init();
void irq_init(); 
void timer_init(int hz);

extern void set_interrupts();
extern void clear_interrupts();
extern void outb(uint16 port, uint8 value);
extern uint8 inb(uint16 port);

#endif /*__IO_MAIN_H_*/
