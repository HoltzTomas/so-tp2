#ifndef _TIME_H_
#define _TIME_H_
#include <stdint.h>
#include <naiveConsole.h>
#include <interrupts.h>

#define SECONDS_REG 0x00
#define MINUTES_REG 0x02
#define HOURS_REG 0x04
#define DAY_REG 0x07
#define MONTH_REG 0x08
#define YEAR_REG 0x09
#define GMT_OFFSET -3
#define NMI_DISABLE_BIT 1
#define REG_NEEDED 6
#define PIT_FREQ 1193182

typedef struct {
    uint8_t seconds;
    uint8_t minutes;
    int8_t hours;
    uint16_t year;
    uint8_t month;
    uint8_t day;
} Timestamp;

// Maneja las interrupciones del timer
void timer_handler();

// Pone el proceso actual a dormir por la cantidad de ticks especificada
void sleep(uint64_t ticks);

// Devuelve la cantidad de ticks desde que se inició el sistema
int ticks_elapsed();

// Establece la frecuencia del timer en ticks por segundo
void setTickFrequency(uint16_t freq);

// Devuelve 1 si el año es bisiesto, 0 si no
int isLeap(uint16_t year);

// Convierte un valor en BCD a decimal
uint8_t bcd_decimal(uint8_t hex);

// Llena la estructura Timestamp con la fecha y hora actuales
void getTime(Timestamp *ts);

#endif
