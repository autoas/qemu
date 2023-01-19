/**
 * SSAS - Simple Smart Automotive Software
 * Copyright (C) 2023 Parai Wang <parai@foxmail.com>
 */
/* ================================ [ INCLUDES  ] ============================================== */
#include "Std_Debug.h"
#include "kernel.h"
/* ================================ [ MACROS    ] ============================================== */
/* ================================ [ TYPES     ] ============================================== */
/* ================================ [ DECLARES  ] ============================================== */
extern void terminal_putchar(char c);
extern void serial_putc(const char c);
/* ================================ [ DATAS     ] ============================================== */
/* ================================ [ LOCALS    ] ============================================== */
static void delay(uint32_t loops) {
  while (loops != 0) {
    loops--;
    __asm__ __volatile__("nop");
  }
}
/* ================================ [ FUNCTIONS ] ============================================== */
void __putchar(char chr) {
  terminal_putchar(chr);
  serial_putc(chr);
}

void StartupHook(void) {
}

TASK(TaskIdle) {
  while (1) {
    delay(10000);
    printf("idle alive\n");
  }
}

int main(int argc, char *argv[]) {
  ASLOG(INFO, ("application build @ %s %s\n", __DATE__, __TIME__));
  StartOS(OSDEFAULTAPPMODE);

  return 0;
}