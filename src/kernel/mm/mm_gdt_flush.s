;*******************************************************************************
; _____   ____ _______    _______ ____  ______  _____                          *
;|  __ \ / __ \__   __|/\|__   __/ __ \|  ____|/ ____|          Copyright 2008 *
;| |__) | |  | | | |  /  \  | | | |  | | |__  | (___              Daniel Bader *
;|  ___/| |  | | | | / /\ \ | | | |  | |  __|  \___ \           Vincenz Doelle *
;| |    | |__| | | |/ ____ \| | | |__| | |____ ____) |    Johannes Schamburger *
;|_|     \____/  |_/_/    \_\_|  \____/|______|_____/          Dmitriy Traytel *
;                                                                              *
;      Practical Oriented TeAching Tool, Operating (and) Educating System      *
;                                                                              *
;                           www.potatoes-project.tk                            *
;*******************************************************************************/

;@author Johannes Schamburger

; based on JamesM's kernel development tutorials

; tells the processor where to find the new GDT
[GLOBAL gdt_flush] ; we have to call gdt_flush() from C

gdt_flush:
        mov eax, [esp+4] ; the pointer to the GDT is passed from C as a parameter.
        lgdt [eax]

        mov ax, 0x10 	; 0x10 is the offset in the GDT to our data segment
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax
        jmp 0x08:.flush ; 0x08 is the offset to our code segment,so we are doing a far 
        				; jump which implicitly changes the code segment (cs) to 0x08;
        				; (0x08 is the offset to the code segment in our GDT)
        				; @see http://wiki.osdev.org/Segmentation#Far_Jump
.flush:
           ret