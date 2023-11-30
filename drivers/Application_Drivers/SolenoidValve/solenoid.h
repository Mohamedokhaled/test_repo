/**
 * @file solenoid.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-09-10
 * 
 * @copyright Copyright (c) 2023
 * 
 */


#ifndef SOLENOID_H_
#define SOLENOID_H_
#include "stdint.h"
/**TODO:: RTC integration**/
/*#include "rtc.h"*/
#include "errorcodes.h"

	


#define EVENTS_ARRAY_NUM	23

typedef enum {
	SOLENOID_STATUS_OFF ,
	SOLENOID_STATUS_ON
}indicator_t;


/* event name enum*/
/* @Warning:	if events number changed this will affect eeprom */
typedef enum{
	/* tampers events name from 0 to 32*/
	EVENT_NAME_CURRENT_REVERSE_L1,			/*bit 0*/
	EVENT_NAME_CURRENT_REVERSE_L2,			/*bit 1*/
	EVENT_NAME_CURRENT_REVERSE_L3,			/*bit 2*/
	EVENT_NAME_LOW_BATTARY,					/*bit 3*/
	EVENT_NAME_BATTARY_REMOVED,				/*bit 4*/
	EVENT_NAME_POWER_UNBALANCE,				/*bit 5*/
	EVENT_NAME_NEUTRAL_CURRENT_ZERO,		/*bit 6*/
	EVENT_NAME_EXT_MAGNETIC_INFLUENCE,		/*bit 7*/
	EVENT_NAME_UPPER_COVER_OPEN,			/*bit 8*/
	EVENT_NAME_TERMINAL_COVER_OPEN,			/*bit 9*/
	EVENT_NAME_POWER_LOAD_OVER,				/*bit 10*/
	EVENT_NAME_CREDIT_OUT,					/*bit 11*/
	EVENT_NAME_MISSING_PHASE_VOLTAGE_L1,	/*bit 12*/
	EVENT_NAME_MISSING_PHASE_VOLTAGE_L2,	/*bit 13*/
	EVENT_NAME_MISSING_PHASE_VOLTAGE_L3,	/*bit 14*/
	EVENT_NAME_SUN_RAISE_SUN_SET,			/*bit 15*/
	EVENT_NAME_REMOTE_CONTROL,				/*bit 16*/
	EVENT_NAME_FORCE_CONNECT,				/*bit 17*/
	EVENT_NAME_FORCE_DISCONNECT,			/*bit 18*/
	EVENT_NAME_PHASE_FAILURE,				/*bit 19*/
	EVENT_NAME_CURRENT_LOAD_OVER,			/*bit 20*/
	EVENT_NAME_CURRENT_LOSS,				/*bit 21*/
	EVENT_NAME_VOLTAGE_LOSS,				/*bit 22*/
	EVENT_NAME_RESERVED_FOR_RELAYCONTROL_1,	/*bit 23*/
	EVENT_NAME_RESERVED_FOR_RELAYCONTROL_2,	/*bit 24*/
	EVENT_NAME_RESERVED_FOR_RELAYCONTROL_3,	/*bit 25*/
	EVENT_NAME_RESERVED_FOR_RELAYCONTROL_4,	/*bit 26*/
	EVENT_NAME_RESERVED_FOR_RELAYCONTROL_5, /*bit 27*/
	EVENT_NAME_RESERVED_FOR_RELAYCONTROL_6, /*bit 28*/
	EVENT_NAME_RESERVED_FOR_RELAYCONTROL_7, /*bit 29*/
	EVENT_NAME_RESERVED_FOR_RELAYCONTROL_8, /*bit 30*/
	EVENT_NAME_RESERVED_FOR_RELAYCONTROL_9, /*bit 31*/
	/* the rest of events*/
	EVENT_NAME_POWER_OFF,
	EVENT_NAME_LONG_POWER_OFF,
	EVENT_NAME_RECHRAGE,
	EVENT_NAME_METER_TRIPPING,
	EVENT_NAME_CURRENT_NEGATIVE_PHASE_SEQUENCE,
	EVENT_NAME_VOLTAGE_NEGATIVE_PHASE_SEQUENCE,
	EVENT_NAME_TEMPERATURE_OVER,
	EVENT_NAME_APPARENT_POWER_OVER_LIMIT,
	EVENT_NAME_NEUTRAL_LOSS,
	EVENT_NAME_APPARENT_DEMAND_OVER_LIMIT,
	EVENT_NAME_APPARENT_POWER_UNBALANCE,
	EVENT_NAME_PARAMETER_UPDATE,
	EVENT_NAME_ADJUSTING_CLOCK,
	EVENT_NAME_BILLING_RESET,
	EVENT_NAME_SEC_AUTH_LOW_FAIL,
	EVENT_NAME_SEC_AUTH_HIGH_ECDSA,
	EVENTS_TOTAL_NUMBER,
}EVENT_NAME;

/* date and time structure for Events*/
typedef struct __attribute__((packed))
{
	uint32_t count;
	struct{
		uint16_t year;
		uint8_t month;
		uint8_t day;
		uint8_t hour;
		uint8_t min;
		uint8_t sec;	
	} DateTime;	
	
}event_t;

/*data status*/
typedef union{
	struct{
		uint32_t CurrentReverseL1        : 1;	/*bit 0*/
		uint32_t CurrentReverseL2        : 1;	/*bit 1*/
		uint32_t CurrentReverseL3	     : 1;	/*bit 2*/
		uint32_t LowBattery		         : 1;	/*bit 3*/
		uint32_t BatteryRemoved			 : 1;	/*bit 4*/
		uint32_t PowerUnbalance	         : 1;	/*bit 5*/
		uint32_t NeutralCurrentZero      : 1;	/*bit 6*/
		uint32_t ExtMagneticInfluence    : 1;	/*bit 7*/
		uint32_t UpperCoverOpen			 : 1;	/*bit 8*/
		uint32_t TerminalCoverOpen		 : 1;	/*bit 9*/
		uint32_t LoadOver			     : 1;	/*bit 10*/
		uint32_t CreditOut				 : 1;	/*bit 11*/
		uint32_t MissingPhaseVoltageL1   : 1;	/*bit 12*/
		uint32_t MissingPhaseVoltageL2   : 1;	/*bit 13*/
		uint32_t MissingPhaseVoltageL3   : 1;	/*bit 14*/
		uint32_t SunRais_SunSet			 : 1;	/*bit 15*/
		uint32_t RemoteControl			 : 1;	/*bit 16*/
		uint32_t ConnectForce			 : 1;	/*bit 17*/
		uint32_t DisconnectForce		 : 1;	/*bit 18*/
		uint32_t PhasesFailure			 : 1;	/*bit 19*/
		uint32_t CurrentOver			 : 1;	/*bit 20*/	
		uint32_t CurrentLoss			 : 1;	/*bit 21*/
		uint32_t VoltageLoss		     : 1;	/*bit 22*/								
		uint32_t reserved1				 : 1;	/*bit 23*/
		uint32_t reserved2				 : 1;	/*bit 24*/
		uint32_t reserved3				 : 1;	/*bit 25*/
		uint32_t reserved4				 : 1;	/*bit 26*/
		uint32_t reserved5				 : 1;	/*bit 27*/
		uint32_t reserved6				 : 1;	/*bit 28*/
		uint32_t reserved7				 : 1;	/*bit 29*/	
		uint32_t reserved8				 : 1;	/*bit 30*/
		uint32_t reserved9				 : 1;	/*bit 31*/							

	}bits;
	
	uint32_t data;
}status_t;


/* Data configuration*/
typedef status_t config_t ;
typedef status_t cmp_t;

typedef  enum{
	SOLENOID_IDLE,
	SOLENOID_OPEN,
	SOLENOID_CLOSE,
	SOLENOID_FEED_BACK_CHECKS,
	SOLENOID_ELEC_BRAKE,
	SOLENOID_NO_OPERATION,

}solenoid_operation_state_t;
typedef struct{
indicator_t status;
indicator_t required_status;
uint8_t enable;
uint8_t timeout;/* time out in (time x 100ms) */
solenoid_operation_state_t operation_state;
uint8_t gpio_state;/*pull up and pull down configured */

}solenoid_t;



extern event_t _event[EVENTS_TOTAL_NUMBER]; 
extern status_t dataStatus;
extern config_t dataConfig;

extern solenoid_t solenoid_control ;
 


/* Functions Declaration*/

/* events Declaration functions*/
void init_event(void);
void set_event(uint8_t eventName);
event_t get_event(uint8_t eventName);
void remove_event(uint8_t eventName);
DLMS_ERROR_CODE relay_RemoteEventReset(uint8_t eventName);

/* Tamper Handlers Declaration functions*/
void magneticTamp_detect_config(void);
void coverTamp_detect_config(void);
void terminalTamp_detect_config(void);
void Battery_removal_status_config(void);

void Alarm_ledIndicator_config(void);
void Tamper_ledIndicator_config(void);
void Buzzer_pin_cofig(void);

/* relay action Declaration functions*/
void solenoid_status_config_cmp_act(void);
void solenoid_control_proc(void);


void solenoid_feedback_callback(solenoid_operation_state_t state);
void solenoid_FB_check(void);


#endif 
