
GLOBAL _cli
GLOBAL _sti
GLOBAL picMasterMask
GLOBAL picSlaveMask
GLOBAL _lidt
GLOBAL haltcpu
GLOBAL _hlt

GLOBAL _irq00Handler
GLOBAL _irq01Handler
GLOBAL _irq02Handler
GLOBAL _irq03Handler
GLOBAL _irq04Handler
GLOBAL _irq05Handler
GLOBAL _syscallHandler


EXTERN irqDispatcher
EXTERN syscallDispatcher
EXTERN idtr
EXTERN next_process

SECTION .text

%macro pushState 0  ; fuente: RowDaBoat/Proyect Wyrm
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
	push fs
	push gs
%endmacro

%macro popState 0
	pop gs
	pop fs
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

%macro irqHandlerMaster 1
	pushState

	mov rdi, %1 ; pasaje de parametro
	call irqDispatcher

	;signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	popState
	iretq
%endmacro

_hlt:
	sti
	hlt
	ret


_cli:
	cli
	ret


_sti:
	sti
	ret


picMasterMask:
	push rbp
    mov rbp, rsp
    mov ax, di
    out	21h,al
    pop rbp
    retn

picSlaveMask:
	push    rbp
    mov     rbp, rsp
    mov     ax, di  ; ax = mascara de 16 bits
    out	0A1h,al
    pop     rbp
    retn

_lidt:				; Carga el IDTR
    push    rbp
    mov     rbp, rsp
    push    rbx
    mov     rbx, rdi ; puntero a IDTR
    lidt    [rbx]          ; carga IDTR
    pop     rbx
    pop     rbp
    retn

;8254 Timer (Timer Tick)
_irq00Handler:
;	irqHandlerMaster 0
	pushState

	mov rdi, rsp
	call next_process

	mov rsp, rax

	mov al, 0x20
	out 0x20, al

	popState

	iretq


;Keyboard
_irq01Handler:
	irqHandlerMaster 1

;Cascade pic never called
_irq02Handler:
	irqHandlerMaster 2

;Serial Port 2 and 4
_irq03Handler:
	irqHandlerMaster 3

;Serial Port 1 and 3
_irq04Handler:
	irqHandlerMaster 4

;USB
_irq05Handler:
	irqHandlerMaster 5

_syscallHandler:
	pushState
	mov rdi,rax ; primer parametro
	mov rsi,rbx ; segundo parametro
	call syscallDispatcher ; en rdx y rcx ya se encuentran los correspondientes valores
	mov [aux], rax
	popState
	mov rax, [aux]
	iretq

haltcpu:
	cli
	hlt
	ret

section .bss
	aux: resq 1
