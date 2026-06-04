GLOBAL sys_read
GLOBAL sys_write
GLOBAL sys_clear
GLOBAL sys_time
GLOBAL sys_ticks
GLOBAL sys_malloc
GLOBAL sys_free
GLOBAL sys_mem_info
GLOBAL throw_zero_division
GLOBAL throw_invalid_opcode

section .text

; Syscall convention:
;   C caller passes: rdi=syscall_nr, rsi=arg1, rdx=arg2, rcx=arg3, r8=arg4, r9=arg5
;   We move syscall_nr into rax, shift args down, and int 80h
;   Return value comes back in rax

%macro do_syscall 1
    push rbp
    mov rbp, rsp
    mov rax, %1
    int 0x80
    mov rsp, rbp
    pop rbp
    ret
%endmacro

; uint64_t sys_read(uint64_t fd, char *buf, uint64_t count)
sys_read:
    do_syscall 0

; uint64_t sys_write(uint64_t fd, const char *buf, uint64_t count)
sys_write:
    do_syscall 1

; void sys_time(Timestamp *ts)
sys_time:
    do_syscall 2

; void sys_clear(uint32_t color)
sys_clear:
    do_syscall 3

; uint64_t sys_ticks(void)
sys_ticks:
    do_syscall 4

; void *sys_malloc(uint64_t size)
sys_malloc:
    do_syscall 5

; void sys_free(void *ptr)
sys_free:
    do_syscall 6

; void sys_mem_info(uint64_t *total, uint64_t *free)
sys_mem_info:
    do_syscall 7

throw_zero_division:
    mov rax, 0
    div rax
    ret

throw_invalid_opcode:
    ud2
    ret

section .note.GNU-stack noalloc noexec nowrite progbits
