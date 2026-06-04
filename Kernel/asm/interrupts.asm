GLOBAL _cli
GLOBAL _sti
GLOBAL picMasterMask
GLOBAL picSlaveMask
GLOBAL haltcpu
GLOBAL _hlt

GLOBAL _irq00Handler
GLOBAL _irq01Handler

GLOBAL _int80Handler

GLOBAL _exception0Handler
GLOBAL _exception6Handler

EXTERN irqDispatcher
EXTERN intDispatcher
EXTERN exceptionDispatcher

SECTION .text

; Push order: rax first (deepest), r15 last (top of stack)
; This means rsp[0] = rax after pushState
%macro pushState 0
	push r15
	push r14
	push r13
	push r12
	push r11
	push r10
	push r9
	push r8
	push rsi
	push rdi
	push rbp
	push rdx
	push rcx
	push rbx
	push rax
%endmacro

%macro popState 0
	pop rax
	pop rbx
	pop rcx
	pop rdx
	pop rbp
	pop rdi
	pop rsi
	pop r8
	pop r9
	pop r10
	pop r11
	pop r12
	pop r13
	pop r14
	pop r15
%endmacro

; Skip rax restore so the C return value in rax is preserved
%macro popStateWithoutRax 0
	add rsp, 8
	pop rbx
	pop rcx
	pop rdx
	pop rbp
	pop rdi
	pop rsi
	pop r8
	pop r9
	pop r10
	pop r11
	pop r12
	pop r13
	pop r14
	pop r15
%endmacro

%macro irqHandlerMaster 1
	pushState

	mov rdi, %1
	mov rsi, rsp
	call irqDispatcher

	mov al, 20h
	out 20h, al

	popState
	iretq
%endmacro

%macro exceptionHandler 1
	pushState

	mov rdi, %1
	mov rsi, rsp
	call exceptionDispatcher

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
	out 21h, al
	pop rbp
	retn

picSlaveMask:
	push rbp
	mov rbp, rsp
	mov ax, di
	out 0A1h, al
	pop rbp
	retn

; Timer tick handler (IRQ0)
_irq00Handler:
	irqHandlerMaster 0

; Keyboard handler (IRQ1)
_irq01Handler:
	irqHandlerMaster 1

; Syscall handler (int 80h)
; Convention: rax = syscall number, args in rdi, rsi, rdx, r10, r8, r9
; Return value in rax (preserved by popStateWithoutRax)
_int80Handler:
	pushState
	mov rdi, rsp
	call intDispatcher
	popStateWithoutRax
	iretq

; Zero Division Exception
_exception0Handler:
	exceptionHandler 0

; Invalid OpCode
_exception6Handler:
	exceptionHandler 6

haltcpu:
	cli
	hlt
	ret

SECTION .bss
	aux resq 1

section .note.GNU-stack noalloc noexec nowrite progbits
