#include "delay.h"
#include "tim.h"


void delay_us(uint16_t delay)
{

	__HAL_TIM_SET_COUNTER(&htim1,0);  /* set the counter value a 0*/
	while (__HAL_TIM_GET_COUNTER(&htim1) < delay); /* wait for the counter to reach the us input in the parameter*/


}


