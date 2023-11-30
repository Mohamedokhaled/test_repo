/*
 * lookup.c
 *
 * Created: 8/1/2023 3:05:56 PM
 *  Author: Youssef
 */ 
#include "eeprom_lookup.h"
#include "eeprom.h"


#include "billing.h"
#include "billing_shared.h"
#include "solenoid.h"
#include "GSM.h"

/**
 * WARNING: if the data members swapped will corrupt data 
*/
eeprom_lookup_t eeprom_lookup [EEPROM_LOOKUP_TOTAL_SIZE] =
{
	
	{
		.index = EEPROM_STRUCT_BILLING_LIFE_TIME_REGISTERS,
		.actual_address = & billing_lifetimeTotalsRegisters,
		.size = sizeof(billing_lifetimeTotalsRegisters),
		
	},
	{
		.index = EEPROM_STRUCT_BILLING_CUSTOMER_REGISTERS,
		.actual_address = & billing_customerTotalsRegisters,
		.size = sizeof(billing_customerTotalsRegisters),
		
	},
	{
		.index = EEPROM_STRUCT_BILLING_CURRENT_STEP_TARIFF_PLAN,
		.actual_address = & billing_currentStepTariffPlan,
		.size = sizeof(billing_currentStepTariffPlan),
		
	},
	{
		.index = EEPROM_STRUCT_BILLING_FUTURE_STEP_TARIFF_PLAN,
		.actual_address = & billing_futureStepTariffPlan,
		.size = sizeof(billing_futureStepTariffPlan),
		
	},
	{
		.index = EEPROM_STRUCT_BILLING_CURRENT_BILLING_CYCLE,
		.actual_address = & billing_currentBillingCycle,
		.size = sizeof(billing_currentBillingCycle),

	},
	{
		.index = EEPROM_ARRAY_BILLING_BILLING_CYCLES_HISTORY,
		.actual_address = & billing_billingCyclesHistory,
		.size =  24 * sizeof(billing_billingCycle),
		
	},
	{
		.index = EEPROM_STRUCT_BILLING_HOLIDAYS,
		.actual_address = & billing_holidays,
		.size = sizeof(billing_holidays),
		
	},
	{
		.index = EEPROM_STRUCT_BILLING_WEEKLENDS,
		.actual_address = & billing_weekends,
		.size = sizeof(billing_weekends),
		
	},
	{
		.index = EEPROM_UINT8_BILLING_START_FRIENDLY_TIME,
		.actual_address = & billing_start_friendlytime,
		.size = sizeof(billing_start_friendlytime),
		
	},

	{
		.index = EEPROM_UINT8_BILLING_END_FRIENDLY_TIME,
		.actual_address = & billing_end_friendlytime,
		.size = sizeof(billing_end_friendlytime),
		
	},

	{
		.index = EEPROM_UINT8_BILLING_FRIENDLYTIME_REVERSED,
		.actual_address = & billing_friendlytime_reversed,
		.size = sizeof(billing_friendlytime_reversed),
		
	},
	{
		.index = EEPROM_ARRAY_RELAY_EVENTS,
		.actual_address = &_event,
		.size = 64 * sizeof(event_t),
		
	},
	{
		.index = EEPROM_STRUCT_RELAY_DATA_CONFIG,
		.actual_address = &dataConfig,
		.size = sizeof(dataConfig),
		
	},
	{
		.index = EEPROM_STRUCT_RELAY_DATA_STATUS,
		.actual_address = &dataStatus,
		.size = sizeof(dataStatus),
		
	},
	{
		.index = EEPROM_ARRAY_APN,
		.actual_address = &apn_name,
		.size = 30,
		
	},

};
