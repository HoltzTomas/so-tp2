#include <stdlib.h>

uint64_t atoi(const char *str) {
    uint64_t result = 0;
    while (*str >= '0' && *str <= '9') {
        result = result * 10 + (*str - '0');
        str++;
    }
    return result;
}

void itoa(uint64_t num, char *str) {
    int i = 0;
    if (num == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }
    while (num > 0) {
        str[i++] = '0' + (num % 10);
        num /= 10;
    }
    str[i] = '\0';

    for (int a = 0, b = i - 1; a < b; a++, b--) {
        char tmp = str[a];
        str[a] = str[b];
        str[b] = tmp;
    }
}

uint64_t abs_val(int64_t x) {
    return x < 0 ? (uint64_t)(-x) : (uint64_t)x;
}

int strcmp(const char *str1, const char *str2) {
    while (*str1 && (*str1 == *str2)) {
        str1++;
        str2++;
    }
    return *(unsigned char *)str1 - *(unsigned char *)str2;
}

uint64_t strlen(const char *str) {
    uint64_t len = 0;
    while (str[len]) len++;
    return len;
}

char *strcpy(char *dest, const char *src) {
    char *ret = dest;
    while ((*dest++ = *src++))
        ;
    return ret;
}

int strncmp(const char *s1, const char *s2, uint64_t n) {
    for (uint64_t i = 0; i < n; i++) {
        if (s1[i] != s2[i] || s1[i] == '\0')
            return (unsigned char)s1[i] - (unsigned char)s2[i];
    }
    return 0;
}

void *memset(void *dest, int c, uint64_t n) {
    uint8_t *d = (uint8_t *)dest;
    while (n--)
        *d++ = (uint8_t)c;
    return dest;
}

void *memcpy(void *dest, const void *src, uint64_t n) {
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;
    while (n--)
        *d++ = *s++;
    return dest;
}
