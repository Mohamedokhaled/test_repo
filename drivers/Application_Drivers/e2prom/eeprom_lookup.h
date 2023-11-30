/*
 * lookup.h
 *
 * Created: 8/1/2023 3:36:47 PM
 *  Author: Youssef
 */ 
#ifndef _EEPROM_LOOKUP_H
#define	_EEPROM_LOOKUP_H

#include <stdint-gcc.h>

/************************************************************************************/
/* WARNING: DO NOT CHANGE ORDER														*/
/*			FOLLOW naming convention												*/
/*			IF changing ORDER is mandatory reset eeprom	lookup must has same order	*/
/************************************************************************************/


typedef enum
{
	EEPROM_STRUCT_BILLING_LIFE_TIME_REGISTERS = 0,
	EEPROM_STRUCT_BILLING_CUSTOMER_REGISTERS,
	EEPROM_STRUCT_BILLING_CURRENT_STEP_TARIFF_PLAN,
	EEPROM_STRUCT_BILLING_FUTURE_STEP_TARIFF_PLAN,
	EEPROM_STRUCT_BILLING_CURRENT_BILLING_CYCLE,
	EEPROM_ARRAY_BILLING_BILLING_CYCLES_HISTORY,
	EEPROM_STRUCT_BILLING_HOLIDAYS,
	EEPROM_STRUCT_BILLING_WEEKLENDS,
	EEPROM_UINT8_BILLING_START_FRIENDLY_TIME,
	EEPROM_UINT8_BILLING_END_FRIENDLY_TIME,
	EEPROM_UINT8_BILLING_FRIENDLYTIME_REVERSED,
	EEPROM_ARRAY_RELAY_EVENTS,
	EEPROM_STRUCT_RELAY_DATA_CONFIG,
	EEPROM_STRUCT_RELAY_DATA_STATUS,
	EEPROM_ARRAY_APN,

	/*add here ----------------_-^-_------------------*/
	EEPROM_LOOKUP_TOTAL_SIZE

}eeprom_lookup_enum;


typedef struct __attribute__((__packed__))
{
	uint8_t* actual_address;
	uint16_t eeprom_address;
	uint16_t size;
	uint8_t index;       /*//known number of attributes*/
}eeprom_lookup_t;

extern eeprom_lookup_t eeprom_lookup [EEPROM_LOOKUP_TOTAL_SIZE];

#endif /*#ifndef _EEPROM_LOOKUP_H*/
