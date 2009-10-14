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
