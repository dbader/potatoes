#ifndef __IO_H_
#define __IO_H_

#include "../include/types.h"

void idt_init();
void isr_init();
void irq_init(); 
void timer_init(int hz);
void monitor_init();
void keyboard_init();
void hd_init();
sint32 get_ticks();

void set_disp(uint32 addr);

void add_shortcut(bool control_flag, bool super_flag, uint8 character, void (*function)());

/**
 * Permits interrupts
 */
extern void set_interrupts();

/**
 * Forbids interrupts
 */
extern void clear_interrupts();

extern void make_syscall(uint16 num, void* syscall_struct);

/**
 * Writes a byte to an io-port.
 * 
 * @param port specifies the port to be written to
 * @param value the value of the byte to be written
 */
extern void outb(uint16 port, uint8 value);

/**
 * Reads a byte from an io-port.
 * 
 * @param port specifies the port to be read from
 * @return the value of the byte that was read from port
 */
extern uint8 inb(uint16 port);

/**
 * Reads words from an io-port.
 * 
 * @param port specifies the port to be read from
 * @param *dest the pointer to the memory where the data should be written to
 * @param num number of words to be read 
 */
extern void repinsw(uint16 port, uint16 *dest, uint32 num);

/**
 * Writess words to an io-port.
 * 
 * @param port specifies the port to be written to
 * @param *src the pointer to the memory where the data should be read from
 * @param num number of words to be written
 */
extern void repoutsw(uint16 port, uint16 *src, uint32 num);

/**
 * Gives the CPU some rest ;-).
 */
extern void halt();

void reactivate_pic();
void monitor_invert();

#endif /*__IO_MAIN_H_*/
