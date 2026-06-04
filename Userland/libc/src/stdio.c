#include <stdio.h>
#include <stdlib.h>

#define MAX_PRINT_BUF 1024
#define MAX_NUM_BUF 32

static void num_to_str(int64_t num, char *buf, int is_signed) {
    int i = 0;
    int neg = 0;
    uint64_t unum;

    if (is_signed && num < 0) {
        neg = 1;
        unum = (uint64_t)(-(num + 1)) + 1;
    } else {
        unum = (uint64_t)num;
    }

    if (unum == 0) {
        buf[i++] = '0';
    } else {
        while (unum > 0) {
            buf[i++] = '0' + (unum % 10);
            unum /= 10;
        }
    }
    if (neg) buf[i++] = '-';
    buf[i] = '\0';

    // Reverse
    for (int a = 0, b = i - 1; a < b; a++, b--) {
        char tmp = buf[a];
        buf[a] = buf[b];
        buf[b] = tmp;
    }
}

static void hex_to_str(uint64_t num, char *buf) {
    char hex[] = "0123456789ABCDEF";
    int i = 0;

    if (num == 0) {
        buf[i++] = '0';
    } else {
        while (num > 0) {
            buf[i++] = hex[num & 0xF];
            num >>= 4;
        }
    }
    buf[i] = '\0';

    for (int a = 0, b = i - 1; a < b; a++, b--) {
        char tmp = buf[a];
        buf[a] = buf[b];
        buf[b] = tmp;
    }
}

uint64_t printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    char buffer[MAX_PRINT_BUF];
    char numbuf[MAX_NUM_BUF];
    uint64_t j = 0;

    for (uint64_t i = 0; fmt[i] != '\0' && j < MAX_PRINT_BUF - 1; i++) {
        if (fmt[i] == '%' && fmt[i + 1] != '\0') {
            i++;
            switch (fmt[i]) {
                case 's': {
                    const char *s = va_arg(args, const char *);
                    while (*s && j < MAX_PRINT_BUF - 1)
                        buffer[j++] = *s++;
                    break;
                }
                case 'd': {
                    int32_t d = va_arg(args, int32_t);
                    num_to_str(d, numbuf, 1);
                    for (int k = 0; numbuf[k] && j < MAX_PRINT_BUF - 1; k++)
                        buffer[j++] = numbuf[k];
                    break;
                }
                case 'u': {
                    uint32_t u = va_arg(args, uint32_t);
                    num_to_str(u, numbuf, 0);
                    for (int k = 0; numbuf[k] && j < MAX_PRINT_BUF - 1; k++)
                        buffer[j++] = numbuf[k];
                    break;
                }
                case 'x': {
                    uint64_t x = va_arg(args, uint64_t);
                    hex_to_str(x, numbuf);
                    for (int k = 0; numbuf[k] && j < MAX_PRINT_BUF - 1; k++)
                        buffer[j++] = numbuf[k];
                    break;
                }
                case 'c': {
                    char c = (char)va_arg(args, int);
                    buffer[j++] = c;
                    break;
                }
                case '%':
                    buffer[j++] = '%';
                    break;
                default:
                    buffer[j++] = '%';
                    if (j < MAX_PRINT_BUF - 1) buffer[j++] = fmt[i];
                    break;
            }
        } else {
            buffer[j++] = fmt[i];
        }
    }
    buffer[j] = '\0';

    va_end(args);
    return sys_write(STDOUT, buffer, j);
}

uint64_t putchar(char c) {
    return sys_write(STDOUT, &c, 1);
}

void puts(const char *str) {
    uint64_t len = strlen(str);
    sys_write(STDOUT, str, len);
    sys_write(STDOUT, "\n", 1);
}

uint8_t getchar(void) {
    char c = 0;
    if (sys_read(STDIN, &c, 1) == 0)
        return (uint8_t)EOF_CHAR;
    return (uint8_t)c;
}

uint64_t readline(char *buf, uint64_t max_len) {
    uint64_t i = 0;
    while (i < max_len - 1) {
        char c = getchar();
        if (c == (uint8_t)EOF_CHAR)
            break;
        if (c == '\n') {
            putchar('\n');
            break;
        }
        if (c == '\b') {
            if (i > 0) {
                i--;
                putchar('\b');
            }
            continue;
        }
        buf[i++] = c;
        putchar(c);
    }
    buf[i] = '\0';
    return i;
}
