#include <keyboardDriver.h>
#include <naiveConsole.h>
#include <videoDriver.h>
#include <registers.h>

uint8_t readKeyFromPort();  // Declaración de la función en ASM

static char buffer[BUFFER_SIZE] = {0};
static int currentKey = 0;
static int nextToRead = 0;
int shift = 0;
int capsLock = 0;
static int extended_prefix = 0; // 1 si el último scancode fue 0xE0 (tecla extendida)
static int ctrl = 0; // estado de Ctrl

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


char isFKey(unsigned int key){
	return (key >= 0x3B && key <= 0x44) || key == 0x57 || key == 0x58;
}

char isSpecialKey(unsigned int key){
	return key == L_SHIFT_PRESS || key == R_SHIFT_PRESS ||
		   key == CAPS_LOCK_PRESS || key == ALT_PRESS || isFKey(key) || key == ESC;
}

char readNext() {
    if (nextToRead == currentKey) {
        return 0; // Buffer vacío
    }
    unsigned char ret = buffer[nextToRead];
    nextToRead = (nextToRead + 1) % BUFFER_SIZE;
    return ret;
}

void keyboard_handler(uint64_t rsp) {
    unsigned int key = readKeyFromPort();
    
	// Manejo de prefijo extendido (0xE0) para flechas y otras teclas
    if (key == 0xE0) {
		extended_prefix = 1;
		return;
	}
    
	if (extended_prefix) {
		// Manejo de modificadores extendidos (Right Ctrl: 0xE0 0x1D / 0xE0 0x9D)
		if (key == 0x1D) { // Right Ctrl press
			ctrl = 1;
			extended_prefix = 0;
			return;
		}
		if (key == 0x9D) { // Right Ctrl release
			ctrl = 0;
			extended_prefix = 0;
			return;
		}
		// Para otras teclas extendidas, ignoramos releases (bit 7) y tratamos makes
		if (key & 0x80) {
			extended_prefix = 0; // release de tecla extendida
			return;
		}
		
        if (key == ARROW_UP || key == ARROW_DOWN || key == ARROW_LEFT || key == ARROW_RIGHT) {
            if (((currentKey + 1) % BUFFER_SIZE) != nextToRead) {
                buffer[currentKey] = (char)(0x80 | key); // marcar como tecla especial fuera de ASCII
                currentKey = (currentKey + 1) % BUFFER_SIZE;
            }
            extended_prefix = 0;
			return;
		}
		// Tecla extendida no manejada: limpiar prefijo y continuar
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
		// Algunas BIOS/teclados pueden generar flechas/PgUp/PgDn como no-extendidas en ciertas configuraciones
        case 0x48: // Arrow Up
        case 0x50: // Arrow Down
        case 0x49: // PgUp
        case 0x51: // PgDn
			// Solo encolar como scancode para userland
			if (((currentKey + 1) % BUFFER_SIZE) != nextToRead) {
                buffer[currentKey] = (char)(0x80 | key); // marcar como especial
				currentKey = (currentKey + 1) % BUFFER_SIZE;
			}
            return;
	}

	if (key & 0x80) {
        return;
    }

	if (key < KEYS && key <= MAX_PRESS_KEY && !isSpecialKey(key)) {
		int index;
		if (keyValues[key][0] >= 'a' && keyValues[key][0] <= 'z') {
			index = capsLock ? !shift : shift;
		} else {
			index = shift;
		}
		char charToAdd = keyValues[key][index];

        if (((currentKey + 1) % BUFFER_SIZE) != nextToRead) {
            buffer[currentKey] = charToAdd;
            currentKey = (currentKey + 1) % BUFFER_SIZE;
        }
	}
}
