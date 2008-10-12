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

; @file
; Some functions needed to read/write the registers CR0 and CR3 (needed for paging) 
; @author Johannes Schamburger

; Returns the contents of the CR0 register.
[GLOBAL read_from_cr0]

read_from_cr0:
	mov eax, cr0
	ret

; Writes the contents of the parameter to CR0.
[GLOBAL write_to_cr0]
write_to_cr0:
	push ebp
	mov ebp, esp
	mov eax, [ebp+8]
	mov cr0,  eax
	pop ebp
	ret

; Returns the contents of CR3.
[GLOBAL read_from_cr3]
read_from_cr3:
	mov eax, cr3
	ret

; Writes the contents of the parameter to CR3.
[GLOBAL write_to_cr3]
write_to_cr3:
	push ebp
	mov ebp, esp
	mov eax, [ebp+8]
	mov cr3, eax
	pop ebp
	ret
