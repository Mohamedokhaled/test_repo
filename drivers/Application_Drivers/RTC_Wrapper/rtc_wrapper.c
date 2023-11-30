#include "rtc_wrapper.h"
#include "rtc.h"
#include "enums.h"

#define YEAR_OFFSET			2000U


RTC_status_t rtc_get_date(uint16_t * year,uint8_t *month,uint8_t *day ,uint8_t *week_Day)
{
    uint8_t retD =  HAL_RTC_GetDate(&hrtc,&sDate,RTC_FORMAT_BIN);
    if ( NULL != year)
    {
         *year 	= sDate.Year + YEAR_OFFSET;
    }
    if ( NULL != month)
    {
        *month	= sDate.Month;
    }
    if ( NULL != day )
    {
        *day = sDate.Date;
    }
    
   if ( NULL != week_Day)
   {
        switch (sDate.WeekDay)
        {
            case RTC_WEEKDAY_SATURDAY:
                *week_Day =DLMS_WEEKDAYS_SATURDAY;
                break;
            case RTC_WEEKDAY_SUNDAY:
                *week_Day =DLMS_WEEKDAYS_SUNDAY;
                break;
            case RTC_WEEKDAY_MONDAY:
                *week_Day =DLMS_WEEKDAYS_MONDAY;
                break;
            case RTC_WEEKDAY_TUESDAY:
                *week_Day =DLMS_WEEKDAYS_TUESDAY;
                break;
            case RTC_WEEKDAY_WEDNESDAY:
                *week_Day =DLMS_WEEKDAYS_WEDNESDAY;
                break;
            case RTC_WEEKDAY_THURSDAY:
                *week_Day =DLMS_WEEKDAYS_THURSDAY;
                break;
            case RTC_WEEKDAY_FRIDAY:
                *week_Day =DLMS_WEEKDAYS_FRIDAY;
                break;
            default:
                break;
        }

   }
   
  
    if (HAL_OK == retD)
    {
        return (RTC_SUCCESS);
    }
    else
    {
        return (RTC_FAIL);
    }
}


RTC_status_t rtc_get_time(uint8_t * hours,uint8_t *minutes ,uint8_t *seconds )
{
    uint8_t retT =  HAL_RTC_GetTime(&hrtc,&sTime,RTC_FORMAT_BIN);
    if ( NULL != hours)
    {
        *hours   = sTime.Hours;
    }
    if ( NULL != minutes )
    {
        *minutes = sTime.Minutes;
    }
    
    if (NULL != seconds)
    {
        *seconds =  sTime.Seconds;
    }

    if (HAL_OK == retT)
    {
        return (RTC_SUCCESS);
    }
    else
    {
        return (RTC_FAIL);
    }
}


RTC_status_t rtc_get_data_time(uint16_t * year,uint8_t *month,uint8_t *day ,uint8_t *week_Day,
                                                    uint8_t * hours,uint8_t *minutes ,uint8_t *seconds)
{
    uint8_t retD =  HAL_RTC_GetDate(&hrtc,&sDate,RTC_FORMAT_BIN);
    if ( NULL != year)
    {
        *year 	= sDate.Year + YEAR_OFFSET;
    }
    if ( NULL != month)
    {
        *month	= sDate.Month;
    }
    if ( NULL != day )
    {
        *day = sDate.Date;
    }
    
   if ( NULL != week_Day)
   {
        switch (sDate.WeekDay)
        {
            case RTC_WEEKDAY_SATURDAY:
                *week_Day =DLMS_WEEKDAYS_SATURDAY;
                break;
            case RTC_WEEKDAY_SUNDAY:
                *week_Day =DLMS_WEEKDAYS_SUNDAY;
                break;
            case RTC_WEEKDAY_MONDAY:
                *week_Day =DLMS_WEEKDAYS_MONDAY;
                break;
            case RTC_WEEKDAY_TUESDAY:
                *week_Day =DLMS_WEEKDAYS_TUESDAY;
                break;
            case RTC_WEEKDAY_WEDNESDAY:
                *week_Day =DLMS_WEEKDAYS_WEDNESDAY;
                break;
            case RTC_WEEKDAY_THURSDAY:
                *week_Day =DLMS_WEEKDAYS_THURSDAY;
                break;
            case RTC_WEEKDAY_FRIDAY:
                *week_Day =DLMS_WEEKDAYS_FRIDAY;
                break;
            default:
                break;
        }

   }
   
   uint8_t retT =  HAL_RTC_GetTime(&hrtc,&sTime,RTC_FORMAT_BIN);
    if ( NULL != hours)
    {
        *hours   = sTime.Hours;
    }
    if ( NULL != minutes )
    {
        *minutes = sTime.Minutes;
    }
    
    if (NULL != seconds)
    {
        *seconds =  sTime.Seconds;
    }
    

    if ((HAL_OK == retD)&&(HAL_OK ==retT))
    {
        return (RTC_SUCCESS);
    }
    else
    {
        return (RTC_FAIL);
    }
}




RTC_status_t rtc_set_date(uint16_t * year,uint8_t *month,uint8_t *day ,uint8_t *week_Day)
{
    if (NULL != year)
    {
         sDate.Year  = *year  - YEAR_OFFSET;
    }
    else
    {
         sDate.Year = 0 ;
    }
    if (NULL != month )
    {
        sDate.Month = *month ;
    }
    else
    {
      sDate.Month = 0; 
    }
    
   if (NULL != day)
   {
      sDate.Date  = *day ;
   }
   else
   {
     sDate.Date = 0;
   }
   
    
    if (NULL != week_Day)
    {
        switch (*week_Day)
        {
            case DLMS_WEEKDAYS_SATURDAY:
            sDate.WeekDay=RTC_WEEKDAY_SATURDAY;
                break;
            case DLMS_WEEKDAYS_SUNDAY:
            sDate.WeekDay=RTC_WEEKDAY_SUNDAY;
                break;
            case DLMS_WEEKDAYS_MONDAY:
            sDate.WeekDay=RTC_WEEKDAY_MONDAY;
                break;
            case DLMS_WEEKDAYS_TUESDAY:
            sDate.WeekDay=RTC_WEEKDAY_TUESDAY;
                break;
            case DLMS_WEEKDAYS_WEDNESDAY:
            sDate.WeekDay=RTC_WEEKDAY_WEDNESDAY;
                break;
            case DLMS_WEEKDAYS_THURSDAY:
            sDate.WeekDay=RTC_WEEKDAY_THURSDAY;
                break;
            case DLMS_WEEKDAYS_FRIDAY:
            sDate.WeekDay=RTC_WEEKDAY_FRIDAY;
                break;
            default:
                break;
        }
        
    }
    else
    {
         sDate.WeekDay=RTC_WEEKDAY_SATURDAY;
    }
    
   
   
    uint8_t retD =  HAL_RTC_SetDate(&hrtc,&sDate,RTC_FORMAT_BIN);

    if (HAL_OK == retD)
    {
        return (RTC_SUCCESS);
    }
    else
    {
        return (RTC_FAIL);
    }

}


RTC_status_t rtc_set_time(uint8_t * hours,uint8_t *minutes ,uint8_t *seconds )
{
    if (NULL != hours)
    {
        sTime.Hours  = *hours ;
    }
    else
    {
         sTime.Hours  = 0;

    }
    if (NULL != minutes)
    {
        sTime.Minutes  = *minutes ;
    }
    else
    {
         sTime.Minutes  = 0;

    }
     if (NULL != seconds)
    {
        sTime.Seconds  = *seconds ;
    }
    else
    {
         sTime.Seconds  = 0;

    }
    
    
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation =RTC_STOREOPERATION_RESET;
    uint8_t retT =  HAL_RTC_SetTime(&hrtc,&sTime,RTC_FORMAT_BIN);

    if (HAL_OK ==retT)
    {
        return (RTC_SUCCESS);
    }
    else
    {
        return (RTC_FAIL);
    }
    
}


RTC_status_t rtc_set_data_time(uint16_t * year,uint8_t *month,uint8_t *day ,uint8_t *week_Day,
                                                    uint8_t * hours,uint8_t *minutes ,uint8_t *seconds)
{
   if (NULL != year)
    {
         sDate.Year  = *year  - YEAR_OFFSET;
    }
    else
    {
         sDate.Year = 0 ;
    }
    if (NULL != month )
    {
        sDate.Month = *month ;
    }
    else
    {
      sDate.Month = 0; 
    }
    
   if (NULL != day)
   {
      sDate.Date  = *day ;
   }
   else
   {
     sDate.Date = 0;
   }
   
    
    if (NULL != week_Day)
    {
        switch (*week_Day)
        {
            case DLMS_WEEKDAYS_SATURDAY:
            sDate.WeekDay=RTC_WEEKDAY_SATURDAY;
                break;
            case DLMS_WEEKDAYS_SUNDAY:
            sDate.WeekDay=RTC_WEEKDAY_SUNDAY;
                break;
            case DLMS_WEEKDAYS_MONDAY:
            sDate.WeekDay=RTC_WEEKDAY_MONDAY;
                break;
            case DLMS_WEEKDAYS_TUESDAY:
            sDate.WeekDay=RTC_WEEKDAY_TUESDAY;
                break;
            case DLMS_WEEKDAYS_WEDNESDAY:
            sDate.WeekDay=RTC_WEEKDAY_WEDNESDAY;
                break;
            case DLMS_WEEKDAYS_THURSDAY:
            sDate.WeekDay=RTC_WEEKDAY_THURSDAY;
                break;
            case DLMS_WEEKDAYS_FRIDAY:
            sDate.WeekDay=RTC_WEEKDAY_FRIDAY;
                break;
            default:
                break;
        }
        
    }
    else
    {
         sDate.WeekDay=RTC_WEEKDAY_SATURDAY;
    }
    
   
   
    uint8_t retD =  HAL_RTC_SetDate(&hrtc,&sDate,RTC_FORMAT_BIN);
   if (NULL != hours)
    {
        sTime.Hours  = *hours ;
    }
    else
    {
         sTime.Hours  = 0;

    }
    if (NULL != minutes)
    {
        sTime.Minutes  = *minutes ;
    }
    else
    {
         sTime.Minutes  = 0;

    }
     if (NULL != seconds)
    {
        sTime.Seconds  = *seconds ;
    }
    else
    {
         sTime.Seconds  = 0;

    }
    
    
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation =RTC_STOREOPERATION_RESET;
    uint8_t retT =  HAL_RTC_SetTime(&hrtc,&sTime,RTC_FORMAT_BIN);
    
   if ((HAL_OK == retD)&&(HAL_OK ==retT))
    {
        return (RTC_SUCCESS);
    }
    else
    {
        return (RTC_FAIL);
    }
}

/* helper function used in day_of_week(uint8_t month, uint8_t day, uint16_t year) */
static int fm(int date, int month, int year) {
	int fmonth, leap;
	if ((year % 100 == 0) && (year % 400 != 0))
		leap = 0;
	else if (year % 4 == 0)
		leap = 1;
	else
		leap = 0;

	fmonth = 3 + (2 - leap) * ((month + 2) / (2 * month))+ (5 * month + month / 9) / 2;

	fmonth = fmonth % 7;

	return fmonth;
}

/* /*day of week, 4th parameter in rtc_set_date() 
 =========================================================
 Description     ::		day of week
 Function        ::		day_of_week
 Input           ::		uint8_t month, uint8_t day, uint16_t year
 Output          ::		dayOfWeek saturday = 0 -> friday = 6
 Call            ::		fm
 Effect          ::
 ========================================================= */
uint8_t day_of_week(uint8_t month, uint8_t day, uint16_t year) {
   uint8_t dayOfWeek;
   int YY = year % 100;
   int century = year / 100;

   dayOfWeek = 1.25 * YY + fm(day, month, year) + day - 2 * (century % 4);

   /*remainder on division by 7*/
   dayOfWeek = dayOfWeek % 7;
   return dayOfWeek;
}
