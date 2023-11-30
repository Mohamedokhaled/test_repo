/*
 * BaseAPI.h
 *
 * Created: 3/28/2023 7:29:27 PM
 *  Author: asamir
 */ 


#ifndef BASEAPI_H_
#define BASEAPI_H_

#include <stdint.h>
#include "Dlms_Interface/Includes/errorcodes.h"
#include "dlms_api.h"
#include "solenoid.h"
#include "billing/billing.h"
#include "Global_register.h"
#include "global_data.h"

#include "EEPROM.h"


typedef struct {
	
	uint8_t Relay;
	uint8_t eeprom;
	uint8_t extFlash;
	uint8_t secureElement;
	uint8_t Backlight;
	uint8_t screen_icons;
	
	}test_mode_status_t;
extern test_mode_status_t test_mode_status;



extern gxRegister AllRegister[max_no_of_registers];
extern gxData AllData[max_no_of_data];
extern gxClock clock_date;
extern gxClock clock_time;

unsigned char* GET_INT_INVOCATION_COUNTER(int* len);

unsigned char* GET_INT_METER_SERIAL(int* len);

unsigned char* GET_INT_DEVICE_ID(int* len);

unsigned char* GET_OCTET_BLOCK_CIPHER_KEY(int* len);

unsigned char* GET_OCTET_AUTHENTICATION_KEY(int* len);

unsigned char* GET_OCTET_KEY_OF_KEYS(int* len);

DLMS_ERROR_CODE SET_Meter_First_Setup(unsigned char* data,int* len);

unsigned char* GET_OCTET_DATA_Customer_ID(int* len);

DLMS_ERROR_CODE SET_OCTET_DATA_Customer_ID(unsigned char* data,int* len);

unsigned char* GET_OCTET_DATA_Meter_Installement_Date(int* len);

DLMS_ERROR_CODE SET_OCTET_DATA_Meter_Installement_Date(unsigned char* data,int* len);

unsigned char* GET_UINT32_DATA_Configure_Meter_Status_Parameters(int* len);

DLMS_ERROR_CODE SET_UINT32_DATA_Configure_Meter_Status_Parameters(unsigned char* data,int* len);

unsigned char* GET_STRUCT_DATA_DISPLAY_ITEMS(int* len);

DLMS_ERROR_CODE SET_STRUCT_DATA_DISPLAY_ITEMS(unsigned char* data,int* len);

unsigned char* GET_STRUCT_DATA_GENERAL_DISPLAY_SETTINGS(int* len);

DLMS_ERROR_CODE SET_STRUCT_DATA_GENERAL_DISPLAY_SETTINGS(unsigned char* data,int* len);

unsigned char* GET_UINT32_DATA_METER_STATUS(int* len);

unsigned char* GET_UINT32_DATA_RECHARGE_COUNT(int* len);

unsigned char* GET_UINT32_DATA_METER_TRIPPING_COUNT(int* len);

unsigned char* GET_UINT32_DATA_PARAMETER_UPDATE_COUNT(int* len);

unsigned char* GET_UINT32_DATA_ADJUSTING_CLOCK_COUNT(int* len);

unsigned char* GET_UINT32_DATA_BILLING_RESET_COUNT(int* len);

unsigned char* GET_UINT32_DATA_ACTIVE_FIRMWARE_IDENTIFIER(int* len);

unsigned char* GET_OCTET_DATA_ACTIVE_FIRMWARE_SIGNATURE(int* len);

unsigned char* GET_UINT32_DATA_COMMON_EVENT_LOGS_FILTER(int* len);

DLMS_ERROR_CODE SET_UINT32_DATA_COMMON_EVENT_LOGS_FILTER(unsigned char* data,int* len);

unsigned char* GET_UINT32_DATA_FRAUD_EVENT_LOGS_FILTER(int* len);

DLMS_ERROR_CODE SET_UINT32_DATA_FRAUD_EVENT_LOGS_FILTER(unsigned char* data,int* len);

unsigned char* GET_UINT32_DATA_FIRMWARE_EVENT_LOGS_FILTER(int* len);

DLMS_ERROR_CODE SET_UINT32_DATA_FIRMWARE_EVENT_LOGS_FILTER(unsigned char* data,int* len);

unsigned char* GET_UINT32_DATA_SYNCHRONIZATION_EVENT_LOGS_FILTER(int* len);

DLMS_ERROR_CODE SET_UINT32_DATA_SYNCHRONIZATION_EVENT_LOGS_FILTER(unsigned char* data,int* len);

unsigned char* GET_UINT32_DATA_PARAMETER_EVENT_LOGS_FILTER(int* len);

DLMS_ERROR_CODE SET_UINT32_DATA_PARAMETER_EVENT_LOGS_FILTER(unsigned char* data,int* len);

unsigned char* GET_STRUCT_DATA_ACTIVE_IMPORT_TARIFF_SETTNIGS(int* len);

unsigned char* GET_STRUCT_DATA_STANDBY_IMPORT_TARIFF_SETTNIGS(int* len);

DLMS_ERROR_CODE SET_STRUCT_DATA_STANDBY_IMPORT_TARIFF_SETTNIGS(unsigned char* data,int* len);

unsigned char* GET_STRUCT_DATA_ACTIVE_EXPORT_TARIFF_SETTNIGS(int* len);

unsigned char* GET_STRUCT_DATA_STANDBY_EXPORT_TARIFF_SETTNIGS(int* len);

DLMS_ERROR_CODE SET_STRUCT_DATA_STANDBY_EXPORT_TARIFF_SETTNIGS(unsigned char* data,int* len);

unsigned char* GET_UINT16_DATA_ACTIVE_IMPORT_TARIFF_CODE(int* len);

unsigned char* GET_UINT16_DATA_STANDBY_IMPORT_TARIFF_CODE(int* len);

DLMS_ERROR_CODE SET_UINT16_DATA_STANDBY_IMPORT_TARIFF_CODE(unsigned char* data,int* len);

unsigned char* GET_UINT16_DATA_ACTIVE_EXPORT_TARIFF_CODE(int* len);

unsigned char* GET_UINT16_DATA_STANDBY_EXPORT_TARIFF_CODE(int* len);

DLMS_ERROR_CODE SET_UINT16_DATA_STANDBY_EXPORT_TARIFF_CODE(unsigned char* data,int* len);

unsigned char* GET_UINT32_DATA_IMPORT_TARIFF_ACTIVATION_DATE(int* len);

DLMS_ERROR_CODE SET_UINT32_DATA_IMPORT_TARIFF_ACTIVATION_DATE(unsigned char* data,int* len);

unsigned char* GET_UINT32_DATA_EXPORT_TARIFF_ACTIVATION_DATE(int* len);

DLMS_ERROR_CODE SET_UINT32_DATA_EXPORT_TARIFF_ACTIVATION_DATE(unsigned char* data,int* len);

unsigned char* GET_INT64_DATA_CUMULATIVE_RECHARGE_AMOUNT(int* len);

unsigned char* GET_INT32_DATA_LAST_RECHARGE_AMOUNT(int* len);

unsigned char* GET_UINT32_DATA_TID(int* len);

unsigned char* GET_INT64_DATA_Remaining_credit(int* len);

unsigned char* GET_UINT64_DATA_Charge_Token_Gateway(int* len);

DLMS_ERROR_CODE SET_UINT64_DATA_Charge_Token_Gateway(unsigned char* data,int* len);

unsigned char* GET_UINT8_DATA_Tariff_Mode(int* len);

DLMS_ERROR_CODE SET_UINT8_DATA_Tariff_Mode(unsigned char* data,int* len);

unsigned char* GET_UINT8_DATA_Tariff_current_step(int* len);

unsigned char* GET_UINT8_DATA_Tamper_reset(int* len);

DLMS_ERROR_CODE SET_UINT8_DATA_Tamper_reset(unsigned char* data,int* len);

unsigned char* GET_STRING_DATA_APN(int* len);

DLMS_ERROR_CODE SET_STRING_DATA_APN(uint8_t * data,uint8_t * len);

unsigned char* GET_STRING_DATA_APN_USERNAME(int* len);

unsigned char* GET_STRING_DATA_APN_PASSWORD(int* len);

unsigned char* GET_UINT8_DATA_weekend(uint8_t* len);

DLMS_ERROR_CODE SET_UINT8_DATA_weekend(unsigned char* data , int* len);

unsigned char* GET_UINT16_DATA_add_holiday(uint8_t* len);

DLMS_ERROR_CODE SET_UINT16_DATA_add_holiday(unsigned char* data , int* len);

unsigned char* GET_UINT16_DATA_remove_holiday(uint8_t* len);

DLMS_ERROR_CODE SET_UINT16_DATA_remove_holiday(unsigned char* data , int* len);

unsigned char* GET_ARR_DATA_all_holidays(uint8_t* len);

DLMS_ERROR_CODE SET_ARR_DATA_all_holidays(unsigned char* data , int* len);

unsigned char* GET_UINT16_DATA_start_friendly_time(uint8_t* len);

DLMS_ERROR_CODE SET_UINT16_DATA_start_friendly_time(unsigned char* data , int* len);

unsigned char* GET_UINT16_DATA_end_friendly_time(uint8_t* len);

DLMS_ERROR_CODE SET_UINT16_DATA_end_friendly_time(unsigned char* data , int* len);

unsigned char* GET_STRUCT_DATA_TEST_MODE(int *len);

DLMS_ERROR_CODE SET_STRUCT_DATA_TEST_MODE(unsigned char* data,int* len);

unsigned char* GET_UINT16_DATA_BILLING_HISTORY_INDEX(int* len);

DLMS_ERROR_CODE SET_UINT16_DATA_BILLING_HISTORY_INDEX(unsigned char* data , int* len);

unsigned char* GET_STUCT_DATA_BILLING_HISTORY(int* len);

DLMS_ERROR_CODE SET_Meter_GLOBAL_RESET(unsigned char* data,int* len);

#endif /* BASEAPI_H_ */
