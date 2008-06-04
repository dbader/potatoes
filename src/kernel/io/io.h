#ifndef __IO_H_
#define __IO_H_

#include "../include/types.h"

void idt_init();
void isr_init();
void irq_init(); 
void timer_init(int hz);
void monitor_init();
void hd_init();
sint32 get_ticks();

extern void set_interrupts();
extern void clear_interrupts();
extern void make_syscall(uint16 num, void* syscall_struct);
extern void outb(uint16 port, uint8 value);
extern uint8 inb(uint16 port);
extern void repinsw(uint16 port, uint16 *dest, uint32 num);
extern void repoutsw(uint16 port, uint16 *src, uint32 num);
extern void halt();

void monitor_invert();

#endif /*__IO_MAIN_H_*/
