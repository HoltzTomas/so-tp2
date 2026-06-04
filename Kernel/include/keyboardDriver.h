#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_
#include <stdint.h>

// Define de teclas especiales:
#define ESC 0x01
#define ENTER 0x1C
#define BACKSPACE 0x0E
#define TAB 0x0F
#define L_SHIFT_PRESS 0x2A
#define L_SHIFT_RELEASE 0xAA
#define R_SHIFT_PRESS 0x36
#define R_SHIFT_RELEASE 0xB6
#define CAPS_LOCK_PRESS 0x3A
#define ALT_PRESS 0x38
#define ALT_RELEASE 0xB8

#define CTRL_PRESS 0x1D
#define CTRL_RELEASE 0x9D
#define KEYBOARD_NULL 0
#define EOF_CHAR ((char)-1)

#define ARROW_UP 0x48
#define ARROW_DOWN 0x50
#define ARROW_LEFT 0x4B
#define ARROW_RIGHT 0x4D

#define KEYS 58
#define MAX_PRESS_KEY 0x70 // Los valores superiores son los release de las teclas
#define TAB_NUM 4
#define BUFFER_SIZE 1000

void keyboard_init(void);
void keyboard_handler(uint64_t rsp);
char readNext();
char readNextBlocking();
char isFKey(unsigned int key);
char isSpecialKey(unsigned int key);

#endif