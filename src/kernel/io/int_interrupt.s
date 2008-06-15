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
;@author Daniel Bader

;********************************************************************************************
; void idt_flush(
[GLOBAL idt_flush]
idt_flush:
        mov eax, [esp+4]  ; Get the pointer to the IDT, passed as a parameter.
        lidt [eax]        ; Load the IDT pointer.
        ret
;********************************************************************************************

; These are some cunning NASM macros to help with setting up
; the ISR and IRQ stubs. They were taken from a tutorial by JamesM.

; A macro to create the stub for regular ISRs
%macro ISR_NOERRCODE 1
  global isr%1
  isr%1:
    cli                         ; Disable interrupts firstly.
    push byte 0                 ; Push a dummy error code.
    push byte %1                ; Push the interrupt number.
    jmp isr_common_stub         ; Go to our common handler code.
%endmacro

; This macro creates a stub for an ISR which passes it's own
; error code.
%macro ISR_ERRCODE 1
  global isr%1
  isr%1:
    cli                         ; Disable interrupts.
    push byte %1                ; Push the interrupt number
    jmp isr_common_stub
%endmacro

; A macro to make the IRQ stubs
%macro IRQ 2
  global irq%1
  irq%1:
    cli
    push byte 0
    push byte %2
    jmp irq_common_stub
%endmacro

;
; Here we setup our ISR stubs
;
ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8
ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_NOERRCODE 17
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31

;
; Setup the IRQ stubs.
;
IRQ   0,    32
IRQ   1,    33
IRQ   2,    34
IRQ   3,    35
IRQ   4,    36
IRQ   5,    37
IRQ   6,    38
IRQ   7,    39
IRQ   8,    40
IRQ   9,    41
IRQ  10,    42
IRQ  11,    43
IRQ  12,    44
IRQ  13,    45
IRQ  14,    46
IRQ  15,    47

[EXTERN isr_handler]
isr_common_stub:
        pushad
        push ds
        push es
        push fs
        push gs

        mov eax, esp
        push eax
        
        mov eax, isr_handler    
        call eax                ; Right now, this will never return...
        
        pop eax

        pop gs
        pop fs
        pop es
        pop ds
        popad
        add esp, 8
        sti
        iret

[EXTERN irq_handler]
irq_common_stub:
        pushad                  ; Push EDI, ESI, EBP, ESP, EBX, EDX, ECX, EAX (32 bytes)
        
        push ds                 ; Push the segment registers (another 16 bytes)
        push es
        push fs
        push gs

        mov eax, esp            ; Push the stack pointer (ESP) without changing it.
        push eax                ; pm_schedule() needs this to save the context
        
        push dword [esp+52]     ; Push the interrupt number our irqX function gave us
        
        call irq_handler        ; irq_handler() returns the adress of the new context,
        mov esp, eax            ; ie stack pointer. Load ESP with that new value.
    
        pop gs                  ; Restore the segment registers (from the new context)
        pop fs
        pop es
        pop ds
        
        popad                   ; Restore all general purpose registers
        
        add esp, 8              ; Remove interrupt number and error code from the stack
        sti
        iret 

;********************************************************************************************
[GLOBAL _syscall]

_syscall:
        int 0x42
        ret

[EXTERN pm_syscall]
[GLOBAL incoming_syscall]	
incoming_syscall:
        cli
        pushad

        push dword [esp+52] ; push syscall data ptr
        push dword [esp+52] ; push syscall id (note that esp was changed by the previous instruction)

        call pm_syscall
        
        add esp, 8
        
        popad
        sti
        iret
        
[EXTERN printhex]
[EXTERN monitor_puti]
[GLOBAL print_stack]
printstack:
        push edx
        push dword [esp+4]
        call printhex
        pop edx
        push dword [esp+8]
        call printhex
        pop edx
        push dword [esp+12]
        call printhex
        pop edx
        push dword [esp+16]
        call printhex
        pop edx
        push dword [esp+20]
        call printhex
        pop edx
        push dword [esp+24]
        call printhex
        pop edx
        push dword [esp+28]
        call printhex
        pop edx
        push dword [esp+32]
        call printhex
        pop edx
        push dword [esp+36]
        call printhex
        pop edx
        push dword [esp+40]
        call printhex
        pop edx
        pop edx
        ret
               
[GLOBAL halt]
halt:
        hlt
        ret
