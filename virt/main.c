#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "kernel.h"
#include "Std_Debug.h"
#include "Std_Timer.h"
#include "Mcu.h"

TASK(Task10Ms) {
  static uint32_t counter = 0;
  counter++;
  if (counter > 500) {
    counter = 0;
    ASLOG(INFO, ("Task10Ms is running\n"));
  }
  TerminateTask();
}

TASK(TaskIdle0) {
  Std_TimerType timer;
  Std_TimerStart(&timer);
  while (1) {
    if (Std_GetTimerElapsedTime(&timer) > 1000000) {
      Std_TimerStart(&timer);
      ASLOG(INFO, ("TaskIdle0 is running\n"));
    }
  }
}

TASK(TaskIdle1) {
  Std_TimerType timer;
  Std_TimerStart(&timer);
  while (1) {
    if (Std_GetTimerElapsedTime(&timer) > 1000000) {
      Std_TimerStart(&timer);
      ASLOG(INFO, ("TaskIdle1 is running\n"));
    }
    Schedule();
  }
}

void StartupHook(void) {
  Mcu_Init(NULL);
}

int main(void) {
  ASLOG(INFO, ("virt build @ %s %s\n", __DATE__, __TIME__));
  StartOS(OSDEFAULTAPPMODE);
  while (1)
    ;
  return 0;
}
