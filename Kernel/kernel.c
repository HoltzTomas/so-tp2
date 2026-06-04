#include <stdint.h>
#include <lib.h>
#include <moduleLoader.h>
#include <naiveConsole.h>
#include <videoDriver.h>
#include <idtLoader.h>
#include <exceptions.h>
#include <memoryManager.h>
#include <scheduler.h>
#include <globals.h>
#include <stddef.h>
#include <semaphore.h>

extern uint8_t text;
extern uint8_t rodata;
extern uint8_t data;
extern uint8_t bss;
extern uint8_t endOfKernelBinary;
extern uint8_t endOfKernel;

static const uint64_t PageSize = 0x1000;
#define STACK_PAGES 8

static void * const sampleCodeModuleAddress = (void*)0x400000;
static void * const sampleDataModuleAddress = (void*)0x500000;

typedef int (*EntryPoint)();

extern int idle_process(int argc, char **argv);
extern void _sti();

void clearBSS(void * bssAddress, uint64_t bssSize) {
	memset(bssAddress, 0, bssSize);
}

void * getStackBase() {
	return (void*)(
		(uint64_t)&endOfKernel
		+ PageSize * STACK_PAGES
		- sizeof(uint64_t)
	);
}

void * initializeKernelBinary() {
	ncPrint("[x64BareBones]");
	ncNewline();

	ncPrint("[Loading modules]");
	ncNewline();
	void * moduleAddresses[] = {
		sampleCodeModuleAddress,
		sampleDataModuleAddress
	};

	loadModules(&endOfKernelBinary, moduleAddresses);
	ncPrint("[Done]");
	ncNewline();
	ncNewline();

	ncPrint("[Initializing kernel's binary]");
	ncNewline();

	clearBSS(&bss, &endOfKernel - &bss);

	return getStackBase();
}

static void initializeMemoryManager() {
	uintptr_t heapStart = (uintptr_t)getStackBase();
	uintptr_t heapEnd = (uintptr_t)sampleCodeModuleAddress;
	uint32_t totalSize = (uint32_t)(heapEnd - heapStart);
	mm_init(heapStart, totalSize);
}

int main() {
	load_idt();
	initializeMemoryManager();

	scheduler_init();
	semaphore_manager_init();

	int16_t default_fds[3] = {STDIN, STDOUT, STDERR};
	create_process(idle_process, NULL, "idle", 0, default_fds, 1);

	EntryPoint entryPoint = (EntryPoint)sampleCodeModuleAddress;
	create_process((MainFunction)entryPoint, NULL, "shell", 2, default_fds, 0);

	_sti();
	yield();

	return 0;
}
