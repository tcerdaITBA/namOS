
GLOBAL time
GLOBAL write
GLOBAL read
GLOBAL paint

section .text

read:
    push rbp
    mov rbp,rsp
    mov rax,3    ; syscall read
    mov rbx,rdi  ; file descriptor
    mov rcx,rsi  ; puntero a buffer
	    ; en rdx ya se encuentra la cantidad de bytes a leer
    int 80h
    mov rsp,rbp
    pop rbp
    ret

write:
	push rbp
	mov rbp,rsp
	mov rax,4    ; syscall write
	mov rbx,rdi  ; file descriptor
	mov rcx,rsi  ; puntero a buffer
		    ; en rdx ya se encuentra la cantidad de bytes a escribir
	int 80h
	mov rsp,rbp
	pop rbp
	ret

time:
    push rbp
    mov rbp,rsp
    mov rax,5    ; syscall time
    mov rbx,rdi  ; segundos, minutos o la hora
    int 80h
    mov rsp,rbp
    pop rbp
    ret

paint:
    push rbp
    mov rbp,rsp
    mov rax,6    ; syscall paint
    mov rbx,rdi  ; color
    mov rcx,rsi  ; posición x
	    ; en rdx ya se encuentra la posición en y
    int 80h
    mov rsp,rbp
    pop rbp
    ret