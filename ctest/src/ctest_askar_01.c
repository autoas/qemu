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
#include "os.h"
#include "ctest.h"

/* ============================ [ MACROS    ] ====================================================== */

/* ============================ [ TYPES     ] ====================================================== */
/* ============================ [ DECLARES  ] ====================================================== */
/* ============================ [ DATAS     ] ====================================================== */
static int T3Counter;
/* ============================ [ LOCALS    ] ====================================================== */
/* ============================ [ FUNCTIONS ] ====================================================== */
int main
(
	void
)
{
	/* start OS in AppMode 1 */
	StartOS(AppMode1);

	/* shall never return */
	while(1);

	return 0;
}
TASK(TaskStart)
{
	static int counter = 0;
	counter ++;

	if(counter > 10000)
	{
		printf(" >> END << \n");
		while(1);
	}
	/* [T1->T3->T5->T2->T6->T4->T3] */
	printf(" >> the %d times test start\n",counter);
	Sequence(0);
	T3Counter = 0;
	ActivateTask(Task1);
	ActivateTask(Task3);
	ActivateTask(Task5);
	ActivateTask(Task2);
	ActivateTask(Task6);
	ActivateTask(Task4);
	ActivateTask(Task3);
	Sequence(1);
	/* Expected [T5->T3->T4->T3->T2->T6->T1] */
	TerminateTask();
}

TASK(Task1)
{
	Sequence(8);
	ChainTask(TaskStart);
}

TASK(Task2)
{
	Sequence(6);
	TerminateTask();
}

TASK(Task3)
{
	if(T3Counter ==0)
	{
		Sequence(3);
	}
	else if(T3Counter ==1)
	{
		Sequence(5);
	}
	else
	{
		ASSERT(OTHER, FAILED);
	}
	T3Counter ++;
	TerminateTask();
}

TASK(Task4)
{
	Sequence(4);
	TerminateTask();
}

TASK(Task5)
{
	Sequence(2);
	TerminateTask();
}

TASK(Task6)
{
	Sequence(7);
	TerminateTask();
}
