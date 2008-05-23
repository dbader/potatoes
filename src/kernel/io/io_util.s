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

[GLOBAL set_interrupts]
set_interrupts:
        sti
        ret
;********************************************************************************************
[GLOBAL clear_interrupts]
clear_interrupts:
        cli
        ret
;********************************************************************************************
[GLOBAL outb]
[GLOBAL inb]

outb:
        mov eax, [esp+8] ;value
        mov edx, [esp+4] ;target address
        out dx, al
        ret
                
inb:
        mov edx, [esp+4] ;source address
        in al, dx
        ret
;********************************************************************************************
[GLOBAL repinsw]

repinsw:
        cld
        mov ecx, [esp+12] ;value
        mov edi, [esp+8] ;dest buffer
        mov edx, [esp+4] ;source address
        rep insw
        ret

;********************************************************************************************
[GLOBAL repoutsw]

repoutsw:
        cld
        mov ecx, [esp+12] ;value
        mov esi, [esp+8] ;source buffer
        mov edx, [esp+4] ;dest address
        rep outsw
        ret