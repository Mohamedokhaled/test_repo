#include "BaseTime.h"
#include "main.h"
#include "task.h"

typedef enum
{
	TIME_100ms = 100,
	TIME_300ms = 300,
	TIME_500ms = 500,
	TIME_1000ms = 1000,
	TIME_1MIN = 59,
	TIME_1HOUR = 59,
	TIME_6HOUR = 6,
	TIME_1DAY = 24,
} TIME_CONSTANS_t;

basetime_t time_tick = {
	.T1ms = 0,
	.T1sec = 0,
	.T1min = 0,
	.T1hr = 0,
	.T1_rx_ms = 0,
	.TO_ms = 0,
};

extern sys_t system_exec;

void basetimer_scheduling_process(void)
{
	/*TODO:: you have to reset WDT here */
	system_exec.ms_ticks = 0;
	time_tick.T1ms++;
	time_tick.T1_rx_ms++;
	time_tick.TO_ms++;
	PutTaskIntoQue(TASK_DLMS_PROC);
	if (0 == (time_tick.T1ms % TIME_100ms))
	{
		PutTaskIntoQue(TASK_TEST_TIMING_100MS);
		PutTaskIntoQue(TASK_CHECK_DISCRETES_PROC);
	}
	if (0 == (time_tick.T1ms % TIME_300ms))
	{
		PutTaskIntoQue(TASK_OPEN_SOLENOID);
		PutTaskIntoQue(TASK_TEST_TIMING_300MS);
	}
	if (0 == (time_tick.T1ms % TIME_500ms))
	{
		PutTaskIntoQue(TASK_SOLENOID_ACT_PROC);
		PutTaskIntoQue(TASK_TEST_TIMING_500MS);
		if (eeprom_get_write_status())
		{
			PutTaskIntoQue(TASK_EEPROM_DYNAMIC_SAVE); /*write periodic data */
		}
	}
	if (time_tick.T1ms >= TIME_1000ms)
	{
		time_tick.T1ms = 0;
		time_tick.T1sec++;
		/* PutTaskIntoQue(TASK_RS485_PROC); */
		PutTaskIntoQue(TASK_READ_RTC);
		PutTaskIntoQue(TASK_SOLENOID_CONTROL_PROC);
		/* PutTaskIntoQue(TASK_EEPROM_READ_BACK); */
	}

	if (time_tick.T1sec > TIME_1MIN)
	{
		time_tick.T1sec = 0;

		PutTaskIntoQue(TASK_BILLING_PROC);
	}
	if (time_tick.T1min > TIME_1HOUR)
	{
		time_tick.T1sec = 0;
		time_tick.T1hr++;
		if (0 == (time_tick.T1hr % TIME_6HOUR))
		{
			eeprom_set_write_status(ENABLE);
			PutTaskIntoQue(TASK_EEPROM_DYNAMIC_SAVE);
		}
		if (TIME_1DAY == time_tick.T1hr)
		{
			time_tick.T1hr = 0;
		}
	}
}
