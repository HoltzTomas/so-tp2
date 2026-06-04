#ifndef NAIVE_CONSOLE_H
#define NAIVE_CONSOLE_H

#include <stdint.h>

extern uint8_t readKey();
extern uint8_t getSeconds();
extern uint8_t getMinutes();
extern uint8_t getHours();
extern uint8_t getDay();
extern uint8_t getMonth();
extern uint8_t getYear();

void ncPrintTime();
void ncPrintKey();
void ncPrintDate();

void ncPrintColorChar(uint8_t color,char character);


void ncPrint(const char * string);
void colorPrint(uint8_t color, const char * string);
void ncPrintChar(char character);
void ncNewline();
void ncPrintDec(uint64_t value);
void ncPrintHex(uint64_t value);
void ncPrintBin(uint64_t value);
void ncPrintBase(uint64_t value, uint32_t base);
void ncClear();

#endif