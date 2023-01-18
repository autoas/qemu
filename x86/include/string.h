#ifndef __STRING_H__
#define __STRING_H__
#include <stdint.h>

size_t strlen(const char *s);
void *memset(void *__s, int __c, size_t __n);
void *memcpy(void *__to, const void *__from, size_t __size);
void *memmove(void *dest, const void *src, size_t len);
void *memchr(const void *s, int c, size_t n);
char *strcpy(char *__to, const char *__from);
char *strcat(char *__to, const char *__from);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
size_t strlcpy(char *dest, const char *src, size_t dest_size);
#endif