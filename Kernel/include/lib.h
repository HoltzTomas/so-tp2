#ifndef LIB_H
#define LIB_H

#include <stdint.h>

// Establece un bloque de memoria con un valor específico
void * memset(void * destination, int32_t character, uint64_t length);

// Copia un bloque de memoria de una ubicación a otra
void * memcpy(void * destination, const void * source, uint64_t length);

// Mueve un bloque de memoria, manejando solapamientos
void * memmove(void *dest, const void *src, uint64_t n);

uint64_t strlen(const char *str);
char *strcpy(char *dest, const char *src);
int strcmp(const char *s1, const char *s2);

int _xchg(int *ptr, int value);

#endif