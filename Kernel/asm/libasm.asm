GLOBAL getSeconds
GLOBAL getMinutes
GLOBAL getHours
GLOBAL getDay
GLOBAL getMonth
GLOBAL getYear
GLOBAL readKeyFromPort
GLOBAL readKey
GLOBAL restore_context

GLOBAL outb
GLOBAL inb
GLOBAL getRSP
GLOBAL getRBP

section .text

getSeconds:
	mov al, 0
	out 70h, al
	in al, 71h
	ret

getMinutes:
	mov al, 2
	out 70h, al
	in al, 71h
	ret

getHours:
	mov al, 4
	out 70h, al
	in al, 71h
	ret

getDay:
	mov al, 7
	out 70h, al
	in al, 71h
	ret

getMonth:
	mov al, 8
	out 70h, al
	in al, 71h
	ret

getYear:
	mov al, 9
	out 70h, al
	in al, 71h
	ret

readKey:
	mov dx, 64h
.wait:
	in al, dx
	test al, 1
	jz .wait
	mov dx, 60h
	in al, dx
	ret

readKeyFromPort:
	xor rax, rax
	in al, 0x60
	ret

outb:
	mov rdx, rdi
	mov rax, rsi
	out dx, al
	ret

inb:
	mov rdx, rdi
	in al, dx
	ret

getRSP:
	mov rax, rsp
	ret

getRBP:
	mov rax, rbp
	ret

restore_context:
	mov rbp, rdx
	mov rsp, rsi
	mov rax, rdi
	jmp rax

section .note.GNU-stack noalloc noexec nowrite progbits
