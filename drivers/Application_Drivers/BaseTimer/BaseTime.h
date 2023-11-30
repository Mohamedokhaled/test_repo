/*
 * BaseTime.h
 *
 *  Created on: Sep 9, 2023
 *      Author: oem
 */

#ifndef APPLICATION_DRIVERS_BASETIMER_BASETIME_H_
#define APPLICATION_DRIVERS_BASETIMER_BASETIME_H_

#include "stdint.h"

typedef struct
{
   uint32_t TO_ms;
   uint32_t T1_rx_ms;
   uint16_t T1ms;
   uint8_t T1sec;
   uint8_t T1min;
   uint8_t T1hr;
} basetime_t;


extern basetime_t time_tick ;

void basetimer_scheduling_process( void );


#endif /* APPLICATION_DRIVERS_BASETIMER_BASETIME_H_ */


