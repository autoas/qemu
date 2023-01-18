#ifndef __STD_IO_H__
#define __STD_IO_H__
#include <stdint.h>

int sprintf(char *buf, const char *format, ...);
int printf(const char *fmt, ...);
int puts(const char *pstr);
int putchar(int c);

void outb(uint32_t port, uint8_t value);
char inb(uint32_t port);
#endif