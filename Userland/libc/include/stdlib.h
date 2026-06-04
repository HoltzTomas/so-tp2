#ifndef STDLIB_H
#define STDLIB_H

#include <stdint.h>

uint64_t atoi(const char *str);
void itoa(uint64_t num, char *str);
uint64_t abs_val(int64_t x);
int strcmp(const char *str1, const char *str2);
uint64_t strlen(const char *str);
char *strcpy(char *dest, const char *src);
int strncmp(const char *s1, const char *s2, uint64_t n);
void *memset(void *dest, int c, uint64_t n);
void *memcpy(void *dest, const void *src, uint64_t n);

#endif
