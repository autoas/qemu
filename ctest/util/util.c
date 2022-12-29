/**
 * AS - the open source Automotive Software on https://github.com/parai
 *
 * Copyright (C) 2017  AS <parai@foxmail.com>
 *
 * This source code is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by the
 * Free Software Foundation; See <http://www.gnu.org/licenses/old-licenses/gpl-2.0.txt>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 */
/* ============================ [ INCLUDES  ] ====================================================== */
#include "Os.h"
/* ============================ [ MACROS    ] ====================================================== */
/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
#ifdef __arch_versatilepb__
extern void serial_send_char(char ch);
void timer_init(void (*cbk)(void));
void timer_stop(void);
void vic_setup(void);
void irq_init(void);
#endif
extern ISR(ISR2);
extern ISR(ISR3);
/* ============================ [ DATAS     ] ====================================================== */
static volatile int isr2Flag;
static volatile int isr3Flag;
static volatile int counterFlag;
static uint32 counterId;
#ifdef __arch_versatilepb__
static volatile int isr2OneShot;
static volatile int isr3OneShot;
static int vicInitFlag = 0;
#endif
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
void __weak ErrorHook(StatusType Error)
{
	
}

void isr2_handler(void)
{
	isr2Flag++;
	printf(" >> ISRMainISR2\n");
	ISRMainISR2();
#ifdef __arch_versatilepb__
	if(isr2OneShot) timer_stop();
#endif
}

void isr3_handler(void)
{
	isr3Flag++;
	printf(" >> ISRMainISR3\n");
	ISRMainISR3();
#ifdef __arch_versatilepb__
	if(isr3OneShot) timer_stop();
#endif

}

void TriggerISR2(void)
{
#ifdef __arch_versatilepb__
	if(0 == vicInitFlag)
	{
		vicInitFlag = 1;
		vic_setup();
		irq_init();
	}
	isr2Flag = 0;
	isr2OneShot=1;
	timer_init(isr2_handler);
	while(isr2Flag == 0);
#endif
}

void TriggerISR3(void)
{
#ifdef __arch_versatilepb__
	if(0 == vicInitFlag)
	{
		vicInitFlag = 1;
		vic_setup();
		irq_init();
	}
	isr3Flag = 0;
	isr3OneShot=1;
	timer_init(isr3_handler);
	while(isr3Flag == 0);
#endif
}
void counter_handler(void)
{
	counterFlag++;

	SignalCounter(counterId);
#ifdef __arch_versatilepb__
	timer_stop();
#endif
}

#ifdef __ASKAR_OS__
uint32 IncrementCounter
(
	uint32 CounterID,
	uint32 Increment
)
{
	counterId = CounterID;
#ifdef __arch_versatilepb__
	if(0 == vicInitFlag)
	{
		vicInitFlag = 1;
		vic_setup();
		irq_init();
	}

	while(Increment>0)
	{
		counterFlag = 0;
		timer_init(counter_handler);
		while(counterFlag == 0);
		Increment --;
	}
#endif
}
#endif

#ifdef MTEST
__weak ISR(Ir_RealTimInt) {}
ISR(SystemTimer)
{
	ISRMainIr_RealTimInt();
#ifdef COUNTER_ID_SystemTimer
	SignalCounter(COUNTER_ID_SystemTimer);
#endif
}
int main()
{
#ifdef __arch_versatilepb__
	if(0 == vicInitFlag)
	{
		vicInitFlag = 1;
		vic_setup();
		irq_init();
	}

	timer_init(ISRMainSystemTimer);
#endif
	StartOS(OSDEFAULTAPPMODE);
	while(1);
}
#endif
