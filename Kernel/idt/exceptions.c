#include <exceptions.h>
#include <videoDriver.h>
#include <registers.h>
#include <keyboardDriver.h>
#include <stdint.h>
#include <interrupts.h>

extern void restore_context(uint64_t rip, uint64_t rsp, uint64_t rbp);

typedef struct {
	uint64_t rip, rsp, rbp;
} restore_point;

static restore_point rp;

static void printRegistersFromStack(const registers_t *regs);

void exceptionDispatcher(int exception, uint64_t *rsp) {
	clear_screen(0);
	switch (exception) {
		case ZERO_EXCEPTION_ID:
			printString("Zero Division Exception\n", 0xFF0000);
			break;
		case INVALID_OPCODE_EXCEPTION_ID:
			printString("Invalid Opcode Exception\n", 0xFF0000);
			break;
		default:
			printString("Unknown exception occurred!\n", 0xFF0000);
			break;
	}
	printRegistersFromStack((const registers_t *)rsp);
	printString("\n\nPress any key to continue...", 0xFFFFFF);
	_sti();
	while (readNext() == 0) {
		_hlt();
	}
	clear_screen(0);
	restore_context(rp.rip, rp.rsp, rp.rbp);
}

static void printRegistersFromStack(const registers_t *regs) {
	char *names[] = {"RAX", "RBX", "RCX", "RDX", "RBP", "RDI", "RSI",
	                 "R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15",
	                 "RIP", "CS", "RFLAGS", "RSP", "SS"};
	uint64_t values[] = {
		regs->rax, regs->rbx, regs->rcx, regs->rdx, regs->rbp,
		regs->rdi, regs->rsi, regs->r8, regs->r9, regs->r10,
		regs->r11, regs->r12, regs->r13, regs->r14, regs->r15,
		regs->rip, regs->cs, regs->rflags, regs->rsp, regs->ss
	};

	printString("\nRegister state:\n", 0x00FF00);
	for (int i = 0; i < 20; i++) {
		printString(names[i], 0xFFFFFF);
		printString(": ", 0xFFFFFF);
		printHex(values[i], 0xFFFFFF);
		printString("\n", 0xFFFFFF);
	}
}

void set_restore_point(uint64_t rip, uint64_t rsp, uint64_t rbp) {
	rp.rip = rip;
	rp.rsp = rsp;
	rp.rbp = rbp;
}
