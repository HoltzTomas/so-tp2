#include <time.h>
#include <keyboardDriver.h>
#include <stdint.h>

void irqDispatcher(uint64_t irq, uint64_t rsp) {
	switch (irq) {
		case 0:
			timer_handler();
			break;
		case 1:
			keyboard_handler(rsp);
			break;
	}
}
