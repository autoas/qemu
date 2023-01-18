/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2023 Parai Wang <parai@foxmail.com>
 */
/* ================================ [ INCLUDES  ] ============================================== */
#include "Std_Debug.h"
/* ================================ [ MACROS    ] ============================================== */
/* ================================ [ TYPES     ] ============================================== */
/* ================================ [ DECLARES  ] ============================================== */
extern void terminal_putchar(char c);
extern void serial_putc(const char c);
/* ================================ [ DATAS     ] ============================================== */
/* ================================ [ LOCALS    ] ============================================== */
/* ================================ [ FUNCTIONS ] ============================================== */
void __putchar(char chr) {
  terminal_putchar(chr);
  serial_putc(chr);
}

int main(int argc, char *argv[]) {
  ASLOG(INFO, ("application build @ %s %s\n", __DATE__, __TIME__));
  while (1)
    ;
  return 0;
}