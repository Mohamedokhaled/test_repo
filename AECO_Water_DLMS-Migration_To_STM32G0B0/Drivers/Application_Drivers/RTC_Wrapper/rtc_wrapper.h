/**
 * @file rtc_wrapper.h
 * @author Mohamed Ibrahem (mohamed.ibrahem@pylonump.com)
 * @brief 
 * 
 * @version 0.1
 * @date 2023-09-13
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef APPLICATION_DRIVERS_RTC_WRAPPER_H_
#define APPLICATION_DRIVERS_RTC_WRAPPER_H_

#include "stdint.h"

typedef enum{
    RTC_FAIL,
    RTC_SUCCESS,

}RTC_status_t;

RTC_status_t rtc_get_date(uint16_t * year,uint8_t *month,uint8_t *day ,uint8_t *week_Day);
RTC_status_t rtc_get_time(uint8_t * hours,uint8_t *minutes ,uint8_t *seconds );
RTC_status_t rtc_get_data_time(uint16_t * year,uint8_t *month,uint8_t *day ,uint8_t *week_Day,
                                                    uint8_t * hours,uint8_t *minutes ,uint8_t *seconds);
RTC_status_t rtc_set_date(uint16_t * year,uint8_t *month,uint8_t *day ,uint8_t *week_Day);
RTC_status_t rtc_set_time(uint8_t * hours,uint8_t *minutes ,uint8_t *seconds );
                             
RTC_status_t rtc_set_data_time(uint16_t * year,uint8_t *month,uint8_t *day ,uint8_t *week_Day,
                                                    uint8_t * hours,uint8_t *minutes ,uint8_t *seconds);

uint8_t day_of_week(uint8_t month, uint8_t day, uint16_t year) ;
#endif /* APPLICATION_DRIVERS_RTC_WRAPPER_H_ */
