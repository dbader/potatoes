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