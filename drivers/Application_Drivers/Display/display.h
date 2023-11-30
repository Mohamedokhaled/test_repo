#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "stdint.h"


#define MAX_SCREENS     255


typedef enum
{
	FORWARD				= 0x05,
	BACKWARD			= 0x50,
	DisplayRam_Max		= 12,
	LPDisplayNum_Max	= 8, /*// one page is added*/


} DISPLAY_CONST;

typedef struct
{
	uint8_t Direct;					/*// direction 0x50:backword,0x05:forward*/
	uint8_t Number;					/*// displaynumber*/
	uint8_t LpTimer;				/*// display loop timer*/
	uint8_t Buff[DisplayRam_Max];	/*// display ram num*/

} DISPLAY_Str;

typedef struct
{
	uint8_t size;
	uint8_t screen_number[237];
}screens_t ;


typedef enum{

	/*data and register ID and attributes*/
	CLASS_ID_DATA=1,
	CLASS_ID_REGISTER=3,
	ATTRIBUTE_ID_LN = 1,
	ATTRIBUTE_ID_VALUE=2,
	ATTRIBUTE_ID_SCALER=3,

	/* CLOCK CLASS ID AND ATTRIBUTES*/
	CLASS_ID_CLOCK = 8,
	ATTRIBUTE_ID_TIME = 2
}SCREEN_GONFIG;



	/* display object function status text replacement*/
	typedef enum{
		DISPLAY_OBJECT_SUCCESS,
		DISPLAY_OBJECT_FAIL

	}DISPLAY_STATUS;


		/*screen config structure*/
		typedef struct{
			uint32_t classID;
			uint8_t logical_name[6];
			uint8_t attribute_id;
			uint8_t numOfDigitBeforeDot;
			uint8_t numOfDigitAfterDot;
		}screen_config_t;

	/* config screen on/off period*/
	typedef struct{
		uint8_t screenPeriod;
		uint8_t LcdOffPeriod;
		uint8_t screen_disp;
		screen_config_t screen[MAX_SCREENS];
	}Lcd_config_t;

	extern Lcd_config_t lcd_config;
	extern uint8_t screen_index;
	extern double lcdperiodTick;
	extern double lcdPeriodOffTick;
 
#endif /* __DISPLAY_H__ */
