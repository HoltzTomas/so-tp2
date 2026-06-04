GLOBAL sys_read
GLOBAL sys_write
GLOBAL sys_clear
GLOBAL sys_time
GLOBAL sys_ticks
GLOBAL sys_malloc
GLOBAL sys_free
GLOBAL sys_mem_info
GLOBAL sys_create_process
GLOBAL sys_getpid
GLOBAL sys_kill
GLOBAL sys_block
GLOBAL sys_unblock
GLOBAL sys_yield
GLOBAL sys_waitpid
GLOBAL sys_nice
GLOBAL sys_ps
GLOBAL sys_sleep
GLOBAL sys_sem_init
GLOBAL sys_sem_open
GLOBAL sys_sem_close
GLOBAL sys_sem_destroy
GLOBAL sys_sem_wait
GLOBAL sys_sem_post
GLOBAL throw_zero_division
GLOBAL throw_invalid_opcode

section .text

; Syscall convention:
;   rax = syscall number, args stay in rdi, rsi, rdx, r10, r8, r9
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

; int64_t sys_create_process(MainFunction code, char **args, char *name, uint8_t priority, int16_t *fds)
; Args: rdi=code, rsi=args, rdx=name, rcx->r10=priority, r8=fds
sys_create_process:
    push rbp
    mov rbp, rsp
    mov r10, rcx        ; move 4th arg from rcx to r10 (syscall convention)
    mov rax, 8
    int 0x80
    mov rsp, rbp
    pop rbp
    ret

; uint64_t sys_getpid(void)
sys_getpid:
    do_syscall 9

; int64_t sys_kill(uint16_t pid, int32_t retval)
sys_kill:
    do_syscall 10

; int64_t sys_block(uint16_t pid)
sys_block:
    do_syscall 11

; int64_t sys_unblock(uint16_t pid)
sys_unblock:
    do_syscall 12

; void sys_yield(void)
sys_yield:
    do_syscall 13

; int64_t sys_waitpid(uint16_t pid)
sys_waitpid:
    do_syscall 14

; int64_t sys_nice(uint16_t pid, uint8_t new_priority)
sys_nice:
    do_syscall 15

; int32_t sys_ps(ProcessInfo *info, uint32_t max_count)
sys_ps:
    do_syscall 16

; void sys_sleep(uint64_t ticks)
sys_sleep:
    do_syscall 17

; int8_t sys_sem_init(uint16_t id, uint32_t initial_value)
sys_sem_init:
    do_syscall 18

; int8_t sys_sem_open(uint16_t id)
sys_sem_open:
    do_syscall 19

; int8_t sys_sem_close(uint16_t id)
sys_sem_close:
    do_syscall 20

; int8_t sys_sem_destroy(uint16_t id)
sys_sem_destroy:
    do_syscall 21

; int8_t sys_sem_wait(uint16_t id)
sys_sem_wait:
    do_syscall 22

; int8_t sys_sem_post(uint16_t id)
sys_sem_post:
    do_syscall 23

throw_zero_division:
    mov rax, 0
    div rax
    ret

throw_invalid_opcode:
    ud2
    ret

section .note.GNU-stack noalloc noexec nowrite progbits
