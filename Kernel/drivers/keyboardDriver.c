#include <keyboardDriver.h>
#include <naiveConsole.h>
#include <videoDriver.h>
#include <registers.h>
#include <scheduler.h>
#include <globals.h>

uint8_t readKeyFromPort();

static char buffer[BUFFER_SIZE] = {0};
static int currentKey = 0;
static int nextToRead = 0;
static int shift = 0;
static int capsLock = 0;
static int extended_prefix = 0;
static int ctrl = 0;

static int16_t keyboard_waiting_pid = -1;

static unsigned char keyValues[KEYS][2] = {
	{0, 0},
	{27, 27},
	{'1', '!'},
	{'2', '@'},
	{'3', '#'},
	{'4', '$'},
	{'5', '%'},
	{'6', '^'},
	{'7', '&'},
	{'8', '*'},
	{'9', '('},
	{'0', ')'},
	{'-', '_'},
	{'=', '+'},
	{'\b', '\b'},
	{9, 9},
	{'q', 'Q'},
	{'w', 'W'},
	{'e', 'E'},
	{'r', 'R'},
	{'t', 'T'},
	{'y', 'Y'},
	{'u', 'U'},
	{'i', 'I'},
	{'o', 'O'},
	{'p', 'P'},
	{'[', '{'},
	{']', '}'},
	{'\n', '\n'},
	{0, 0},
	{'a', 'A'},
	{'s', 'S'},
	{'d', 'D'},
	{'f', 'F'},
	{'g', 'G'},
	{'h', 'H'},
	{'j', 'J'},
	{'k', 'K'},
	{'l', 'L'},
	{';', ':'},
	{39, 34},
	{'`', '~'},
	{0, 0},
	{'\\', '|'},
	{'z', 'Z'},
	{'x', 'X'},
	{'c', 'C'},
	{'v', 'V'},
	{'b', 'B'},
	{'n', 'N'},
	{'m', 'M'},
	{',', '<'},
	{'.', '>'},
	{'/', '?'},
	{0, 0},
	{0, 0},
	{0, 0},
	{' ', ' '},
};

void keyboard_init(void) {
	keyboard_waiting_pid = -1;
}

char isFKey(unsigned int key){
	return (key >= 0x3B && key <= 0x44) || key == 0x57 || key == 0x58;
}

char isSpecialKey(unsigned int key){
	return key == L_SHIFT_PRESS || key == R_SHIFT_PRESS ||
		   key == CAPS_LOCK_PRESS || key == ALT_PRESS || isFKey(key) || key == ESC;
}

static void enqueue_char(char c) {
	if (((currentKey + 1) % BUFFER_SIZE) != nextToRead) {
		buffer[currentKey] = c;
		currentKey = (currentKey + 1) % BUFFER_SIZE;

		if (keyboard_waiting_pid >= 0) {
			set_status((uint16_t)keyboard_waiting_pid, READY);
			keyboard_waiting_pid = -1;
		}
	}
}

char readNext() {
    if (nextToRead == currentKey) {
        return 0;
    }
    unsigned char ret = buffer[nextToRead];
    nextToRead = (nextToRead + 1) % BUFFER_SIZE;
    return ret;
}

char readNextBlocking() {
	char c = readNext();
	while (c == 0) {
		keyboard_waiting_pid = (int16_t)get_pid();
		set_status(get_pid(), BLOCKED);
		yield();
		c = readNext();
	}
	return c;
}

void keyboard_handler(uint64_t rsp) {
    unsigned int key = readKeyFromPort();
    
	if (key == 0xE0) {
		extended_prefix = 1;
		return;
	}
    
	if (extended_prefix) {
		if (key == 0x1D) {
			ctrl = 1;
			extended_prefix = 0;
			return;
		}
		if (key == 0x9D) {
			ctrl = 0;
			extended_prefix = 0;
			return;
		}
		if (key & 0x80) {
			extended_prefix = 0;
			return;
		}
		
        if (key == ARROW_UP || key == ARROW_DOWN || key == ARROW_LEFT || key == ARROW_RIGHT) {
			enqueue_char((char)(0x80 | key));
            extended_prefix = 0;
			return;
		}
		extended_prefix = 0;
	}
    
    switch (key){
        case R_SHIFT_PRESS:
        case L_SHIFT_PRESS:
            shift = 1;
            return;
        case R_SHIFT_RELEASE:
        case L_SHIFT_RELEASE:
            shift = 0;
            return;
        case CAPS_LOCK_PRESS:
            capsLock = 1 - capsLock;
            return;
		case CTRL_PRESS:
			ctrl = 1;
			return;
		case CTRL_RELEASE:
			ctrl = 0;
			return;
        case 0x48:
        case 0x50:
        case 0x49:
        case 0x51:
			enqueue_char((char)(0x80 | key));
            return;
	}

	if (key & 0x80) {
        return;
    }

	if (ctrl && key < KEYS && key <= MAX_PRESS_KEY) {
		char base = keyValues[key][0];
		if (base == 'c') {
			kill_foreground_process();
			return;
		}
		if (base == 'd') {
			enqueue_char(EOF_CHAR);
			return;
		}
	}

	if (key < KEYS && key <= MAX_PRESS_KEY && !isSpecialKey(key)) {
		int index;
		if (keyValues[key][0] >= 'a' && keyValues[key][0] <= 'z') {
			index = capsLock ? !shift : shift;
		} else {
			index = shift;
		}
		char charToAdd = keyValues[key][index];
		enqueue_char(charToAdd);
	}
}
