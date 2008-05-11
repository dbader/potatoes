;
;      _   _  ____   _____ 
;     | | (_)/ __ \ / ____|
;  ___| |_ _| |  | | (___  
; / _ \ __| | |  | |\___ \  Copyright 2008 Daniel Bader, Vincenz Doelle,
;|  __/ |_| | |__| |____) |        Johannes Schamburger, Dmitriy Traytel
; \___|\__|_|\____/|_____/ 
;
;This program is free software: you can redistribute it and/or modify
;it under the terms of the GNU General Public License as published by
;the Free Software Foundation, either version 3 of the License, or
;(at your option) any later version.
;
;This program is distributed in the hope that it will be useful,
;but WITHOUT ANY WARRANTY; without even the implied warranty of
;MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;GNU General Public License for more details.
;
;You should have received a copy of the GNU General Public License
;along with this program.  If not, see <http://www.gnu.org/licenses/>.

;@author Dmitriy Traytel

	;Install our idt-table:
[GLOBAL idt_load]
[EXTERN idtp]
idt_load:
	lidt [idtp]
	ret
;********************************************************************************************
	;Install the interrupt service routines:
[GLOBAL isr0]
[GLOBAL isr1]
[GLOBAL isr2]
[GLOBAL isr3]
[GLOBAL isr4]
[GLOBAL isr5]
[GLOBAL isr6]
[GLOBAL isr7]
[GLOBAL isr8]
[GLOBAL isr9]
[GLOBAL isr10]
[GLOBAL isr11]
[GLOBAL isr12]
[GLOBAL isr13]
[GLOBAL isr14]
[GLOBAL isr15]
[GLOBAL isr16]
[GLOBAL isr17]
[GLOBAL isr18]
[GLOBAL isr19]
[GLOBAL isr20]
[GLOBAL isr21]
[GLOBAL isr22]
[GLOBAL isr23]
[GLOBAL isr24]
[GLOBAL isr25]
[GLOBAL isr26]
[GLOBAL isr27]
[GLOBAL isr28]
[GLOBAL isr29]
[GLOBAL isr30]
[GLOBAL isr31]

;divide by zero
isr0:
	cli
	push byte 0
	push byte 0
	jmp isr_handler
	
;debug
isr1:
	cli
	push byte 0
	push byte 1
	jmp isr_handler

;non maskable interrupt
isr2:
	cli
	push byte 0
	push byte 2
	jmp isr_handler

;breakpoint
isr3:
	cli
	push byte 0
	push byte 3
	jmp isr_handler
	
;overflow
isr4:
	cli
	push byte 0
	push byte 4
	jmp isr_handler
	
;out of bounds
isr5:
	cli
	push byte 0
	push byte 5
	jmp isr_handler

;invalid opcode
isr6:
	cli
	push byte 0
	push byte 6
	jmp isr_handler
	
;no coprocessor
isr7:
	cli
	push byte 0
	push byte 7
	jmp isr_handler

;double fault (with error code)
isr8:
	cli
	push byte 8
	jmp isr_handler

;coprocessor segment overrun
isr9:
	cli
	push byte 0
	push byte 9
	jmp isr_handler
	
;bad tss (with error code)
isr10:
	cli
	push byte 10
	jmp isr_handler
	
;segment not present (with error code)
isr11:
	cli
	push byte 11
	jmp isr_handler
	
;stack fault (with error code)
isr12:
	cli
	push byte 12
	jmp isr_handler
	
;general protection fault (with error code)
isr13:
	cli
	push byte 13
	jmp isr_handler

;page fault (with error code)
isr14:
	cli
	push byte 14
	jmp isr_handler

;unknown interrupt
isr15:
	cli
	push byte 0
	push byte 15
	jmp isr_handler
	
;coprocessor fault
isr16:
	cli
	push byte 0
	push byte 16
	jmp isr_handler
	
;reserved
isr17:
	cli
	push byte 0
	push byte 17
	jmp isr_handler
	
;reserved
isr18:
	cli
	push byte 0
	push byte 18
	jmp isr_handler

;reserved
isr19:
	cli
	push byte 0
	push byte 19
	jmp isr_handler

;reserved
isr20:
	cli
	push byte 0
	push byte 20
	jmp isr_handler
	
;reserved
isr21:
	cli
	push byte 0
	push byte 21
	jmp isr_handler
;reserved
isr22:
	cli
	push byte 0
	push byte 22
	jmp isr_handler
	
;reserved
isr23:
	cli
	push byte 0
	push byte 23
	jmp isr_handler

;reserved
isr24:
	cli
	push byte 0
	push byte 24
	jmp isr_handler

;reserved
isr25:
	cli
	push byte 0
	push byte 25
	jmp isr_handler
	
;reserved
isr26:
	cli
	push byte 0
	push byte 26
	jmp isr_handler

;reserved
isr27:
	cli
	push byte 0
	push byte 27
	jmp isr_handler
	
;reserved
isr28:
	cli
	push byte 0
	push byte 28
	jmp isr_handler

;reserved
isr29:
	cli
	push byte 0
	push byte 29
	jmp isr_handler

;reserved
isr30:
	cli
	push byte 0
	push byte 30
	jmp isr_handler
	
;reserved
isr31:
	cli
	push byte 0
	push byte 31
	jmp isr_handler

[EXTERN ex_handler]

isr_handler:
	pusha
	push ds
	push es
	push fs
	push gs
	mov ax, 0x10 ;kernel data segment
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	;jump over pushed registers (gs, fs, es, ds, edi, esi, ebp, esp, ebx, edx, ecx, eax): 12*4=48
	push dword [esp + 48] ;function argument
	call ex_handler
	pop eax
	pop gs
	pop fs
	pop es
	pop ds
	popa
	add esp, 8 ;clean up stack
;	add dword [esp], 4 ;jump over the interrupt-causing source code
	iret
;********************************************************************************************
	