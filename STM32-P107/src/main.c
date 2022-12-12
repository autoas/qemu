#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "gpio.h"
#include "kernel.h"
#include "Std_Timer.h"

extern TickType OsTickCounter;

uint32_t Mcu_GetSystemClock(void) {
  return 64000000;
}

std_time_t Std_GetTime(void) {
  return OsTickCounter * USECONDS_PER_TICK;
}

TASK(Task10Ms) {
  static uint32_t counter = 0;
  counter++;
  if (counter > 50) {
    led2_toggle();
    counter = 0;
  }
  TerminateTask();
}

TASK(TaskIdle) {
  Std_TimerType timer;
  Std_TimerStart(&timer);
  while (1) {
    if (Std_GetTimerElapsedTime(&timer) > 1000000) {
      led1_toggle();
      Std_TimerStart(&timer);
    }
  }
}

void StartupHook(void) {
}

int main(void) {
  gpio_init();
  StartOS(OSDEFAULTAPPMODE);
  while (1)
    ;
  return 0;
}
