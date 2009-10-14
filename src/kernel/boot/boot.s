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

;
; boot.s -- Kernel start location. Also defines multiboot header.
;           Based on Bran's kernel development tutorial file start.asm
;

MBOOT_PAGE_ALIGN    equ 1<<0    ; Load kernel and modules on a page boundary
MBOOT_MEM_INFO      equ 1<<1    ; Provide your kernel with memory info
MBOOT_HEADER_MAGIC  equ 0x1BADB002 ; Multiboot Magic value
; NOTE: We do not use MBOOT_AOUT_KLUDGE. It means that GRUB does not
; pass us a symbol table.
MBOOT_HEADER_FLAGS  equ MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO
MBOOT_CHECKSUM      equ -(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)


[BITS 32]                       ; All instructions should be 32-bit.

[GLOBAL mboot]                  ; Make 'mboot' accessible from C.
[EXTERN code]                   ; Start of the '.text' section.
[EXTERN bss]                    ; Start of the .bss section.
[EXTERN end]                    ; End of the last loadable section.

mboot:
    dd  MBOOT_HEADER_MAGIC      ; GRUB will search for this value on each
                                ; 4-byte boundary in your kernel file
    dd  MBOOT_HEADER_FLAGS      ; How GRUB should load your file / settings
    dd  MBOOT_CHECKSUM          ; To ensure that the above values are correct
; fields used if MULTIBOOT_AOUT_KLUDGE is set in MULTIBOOT_HEADER_FLAGS
;    dd  mboot                   ; Location of this descriptor
;    dd  code                    ; Start of kernel '.text' (code) section.
;    dd  bss                     ; End of kernel '.data' section.
;    dd  end                     ; End of kernel.
;    dd  start                   ; Kernel entry point (initial EIP).

[GLOBAL start]                  ; Kernel entry point.
[GLOBAL end]
[EXTERN main]                   ; This is the entry point of our C code

start:

        ; Execute the kernel:
        cli

        ; Relocate the stack.
        ; FIXME: we should somehow choose a proper adress. Right now this is kind of a hack.
        mov esp, 0x300000
        mov ebp, esp
        ;cli
        ; Load multiboot information:
        push ebx
        cli                     ;Disable interrupts
        call main               ; call our main() function.
        jmp $                   ; Enter an infinite loop, to stop the processor
                                ; executing whatever rubbish is in the memory
                                ; after our kernel!
