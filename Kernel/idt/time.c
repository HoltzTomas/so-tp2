#include <time.h>
#include <interrupts.h>

static unsigned long ticks = 0;

extern uint8_t outb(uint16_t port, uint8_t value);
extern uint8_t inb(uint16_t port);

void timer_handler() {
    ticks++;
}

int ticks_elapsed() {
    return ticks;
}

int isLeap(uint16_t year) {
    return year % 4 == 0 && (year % 100 != 0 || year % 400 == 0);
}

uint8_t bcd_decimal(uint8_t hex) {
    int dec = ((hex & 0xF0) >> 4) * 10 + (hex & 0x0F);
    return dec;
}

void getTime(Timestamp *ts) {
    uint8_t daysPerMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    uint8_t regNeeded[] = {SECONDS_REG, MINUTES_REG, HOURS_REG, DAY_REG, MONTH_REG, YEAR_REG};
    uint8_t data[REG_NEEDED];
    for (int i = 0; i < REG_NEEDED; i++) {
        outb(0x70, (NMI_DISABLE_BIT << 7) | (regNeeded[i]));
        data[i] = inb(0x71);
    }
    ts->seconds = bcd_decimal(data[0]);
    ts->minutes = bcd_decimal(data[1]);
    ts->hours = bcd_decimal(data[2]) + GMT_OFFSET;
    if (ts->hours < 0)
        ts->hours += 24;
    ts->day = bcd_decimal(data[3]);
    if (ts->hours > 21) {
        ts->day--;
        if (ts->day == 0) {
            if (ts->month == 1) {
                ts->month = 12;
                ts->year--;
            } else {
                ts->month--;
            }
            ts->day = daysPerMonth[ts->month - 1] + (ts->month == 2) * isLeap(ts->year);
        }
    }
    ts->month = bcd_decimal(data[4]);
    ts->year = bcd_decimal(data[5]);
}
