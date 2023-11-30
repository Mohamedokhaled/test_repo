/*
* global_data.c
*
* Created: 3/28/2023 7:04:15 PM
*  Author: asamir
*/

#include "global_data.h";
#include "MCU_interface/BaseAPI.h"
#include "Dlms_Interface/Includes/gxmem.h"
#include "billing/billing.h"

int _init_arra_data(obj_data * arr_obj){
	
	/* 
	- obj_register arr_obj_register[max_no_of_registers];
 
	- please add new objects at the end */
	
	arr_obj[DATA_INVOCATION_COUNTER] = (obj_data)
	{
		.obis = {0,0,43,1,0,255},
		.DDT = DLMS_DATA_TYPE_UINT32,
		.read_api = &GET_INT_INVOCATION_COUNTER,
		.write_api = NULL,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};
	arr_obj[DATA_METER_SERIAL] = ( obj_data)
	{
		.obis = {1,1,0,0,1,255},
		.DDT = DLMS_DATA_TYPE_UINT32,
		.read_api = &GET_INT_METER_SERIAL,
		.write_api = NULL
	};
	arr_obj[DATA_DEVICE_ID] = ( obj_data)
	{
		.obis = {1,0,96,1,0,255},
		.DDT = DLMS_DATA_TYPE_UINT32,
		.read_api = &GET_INT_DEVICE_ID,
		.write_api = NULL
	};
	arr_obj[DATA_BLOCK_CIPHER_KEY] = ( obj_data)
	{
		.obis = {0,128,2,0,0,255},
		.DDT = DLMS_DATA_TYPE_OCTET_STRING,
		.read_api = &GET_OCTET_BLOCK_CIPHER_KEY,
		.write_api = NULL
	};
	arr_obj[DATA_AUTHENTICATION_KEY] = ( obj_data)
	{
		.obis = {0,128,3,0,0,255},
		.DDT = DLMS_DATA_TYPE_OCTET_STRING,
		.read_api = &GET_OCTET_AUTHENTICATION_KEY,
		.write_api = NULL
	};
	arr_obj[DATA_KEY_OF_KEYS] = ( obj_data)
	{
		.obis = {0,128,4,0,0,255},
		.DDT = DLMS_DATA_TYPE_OCTET_STRING,
		.read_api = &GET_OCTET_KEY_OF_KEYS,
		.write_api = NULL
	};
	
	
	arr_obj[DATA_METER_SETUP] = ( obj_data)
	{
		.obis = {1,0,0,4,250,255},
		.DDT = DLMS_DATA_TYPE_UINT16,
		.read_api = NULL,
		.write_api = &SET_Meter_First_Setup,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};
	arr_obj[DATA_CUSTOMER_ID] = ( obj_data)
	{
		.obis = {1,0,96,7,0,255},
		.DDT = DLMS_DATA_TYPE_OCTET_STRING,
		.read_api = &GET_OCTET_DATA_Customer_ID,
		.write_api = &SET_OCTET_DATA_Customer_ID,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};
	
	arr_obj[DATA_METER_INSTALLEMENT_DATE] = ( obj_data)
	{
		.obis = {0,0,96,60,22,255},
		.DDT = DLMS_DATA_TYPE_OCTET_STRING,
		.read_api = &GET_OCTET_DATA_Meter_Installement_Date,
		.write_api = &SET_OCTET_DATA_Meter_Installement_Date,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};
	
	arr_obj[DATA_CONFIGURE_METER_STATUS_PARAMETERS] = ( obj_data)
	{
		.obis = {1,0,96,50,70,255},
		.DDT = DLMS_DATA_TYPE_UINT32,
		.read_api = &GET_UINT32_DATA_Configure_Meter_Status_Parameters,
		.write_api = &SET_UINT32_DATA_Configure_Meter_Status_Parameters
	};

	arr_obj[DATA_DISPLAY_ITEMS] = ( obj_data)
	{
		.obis = {0,0,128,0,1,1},
		.DDT = DLMS_DATA_TYPE_STRUCTURE,
		.read_api = &GET_STRUCT_DATA_DISPLAY_ITEMS,
		.write_api = &SET_STRUCT_DATA_DISPLAY_ITEMS
	};

	arr_obj[DATA_GENERAL_DISPLAY_SETTINGS] = ( obj_data)
	{
		.obis = {0,0,128,0,4,1},
		.DDT = DLMS_DATA_TYPE_STRUCTURE,
		.read_api = &GET_STRUCT_DATA_GENERAL_DISPLAY_SETTINGS,
		.write_api = &SET_STRUCT_DATA_GENERAL_DISPLAY_SETTINGS,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};

	arr_obj[DATA_METER_STATUS] = ( obj_data)
	{
		.obis = {1,0,96,51,60,255},
		.DDT = DLMS_DATA_TYPE_UINT32,
		.read_api = &GET_UINT32_DATA_METER_STATUS,
		.write_api = NULL,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};

	arr_obj[DATA_RECHARGE_COUNT] = ( obj_data)
	{
		.obis = {0,0,96,15,0,255},
		.DDT = DLMS_DATA_TYPE_UINT32,
		.read_api = &GET_UINT32_DATA_RECHARGE_COUNT,
		.write_api = NULL,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};

	

	arr_obj[DATA_METER_TRIPPING_COUNT] = ( obj_data)
	{
		.obis = {0,0,96,15,6,255},
		.DDT = DLMS_DATA_TYPE_UINT32,
		.read_api = &GET_UINT32_DATA_METER_TRIPPING_COUNT,
		.write_api = NULL,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};

	
	arr_obj[DATA_PARAMETER_UPDATE_COUNT] = ( obj_data)
	{
		.obis = {0,0,96,15,38,255},
		.DDT = DLMS_DATA_TYPE_UINT32,
		.read_api = &GET_UINT32_DATA_PARAMETER_UPDATE_COUNT,
		.write_api = NULL,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};

	arr_obj[DATA_ADJUSTING_CLOCK_COUNT] = ( obj_data)
	{
		.obis = {0,0,96,15,39,255},
		.DDT = DLMS_DATA_TYPE_UINT32,
		.read_api = &GET_UINT32_DATA_ADJUSTING_CLOCK_COUNT,
		.write_api = NULL,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};

	arr_obj[DATA_BILLING_RESET_COUNT] = ( obj_data)
	{
		.obis = {0,0,96,15,42,255},
		.DDT = DLMS_DATA_TYPE_UINT32,
		.read_api = &GET_UINT32_DATA_BILLING_RESET_COUNT,
		.write_api = NULL,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};

	arr_obj[DATA_ACTIVE_FIRMWARE_IDENTIFIER] = ( obj_data)
	{
		.obis = {1,0,0,2,0,255},
		.DDT = DLMS_DATA_TYPE_UINT32,
		.read_api = &GET_UINT32_DATA_ACTIVE_FIRMWARE_IDENTIFIER,
		.write_api = NULL,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};

	arr_obj[DATA_ACTIVE_FIRMWARE_SIGNATURE] = ( obj_data)
	{
		.obis = {1,0,0,2,8,255},
		.DDT = DLMS_DATA_TYPE_OCTET_STRING,
		.read_api = &GET_OCTET_DATA_ACTIVE_FIRMWARE_SIGNATURE,
		.write_api = NULL,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};

	arr_obj[DATA_COMMON_EVENT_LOGS_FILTER] = ( obj_data)
	{
		.obis = {0,0,94,20,17,255},
		.DDT = DLMS_DATA_TYPE_UINT32,
		.read_api = &GET_UINT32_DATA_COMMON_EVENT_LOGS_FILTER,
		.write_api = &SET_UINT32_DATA_COMMON_EVENT_LOGS_FILTER,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};

	arr_obj[DATA_FRAUD_EVENT_LOGS_FILTER] = ( obj_data)
	{
		.obis = {0,0,94,20,11,255},
		.DDT = DLMS_DATA_TYPE_UINT32,
		.read_api = &GET_UINT32_DATA_FRAUD_EVENT_LOGS_FILTER,
		.write_api = &SET_UINT32_DATA_FRAUD_EVENT_LOGS_FILTER,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};

	arr_obj[DATA_FIRMWARE_EVENT_LOGS_FILTER] = ( obj_data)
	{
		.obis = {0,0,94,20,14,255},
		.DDT = DLMS_DATA_TYPE_UINT32,
		.read_api = &GET_UINT32_DATA_FIRMWARE_EVENT_LOGS_FILTER,
		.write_api = &SET_UINT32_DATA_FIRMWARE_EVENT_LOGS_FILTER,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};

	arr_obj[DATA_SYNCHRONIZATION_EVENT_LOGS_FILTER] = ( obj_data)
	{
		.obis = {0,0,94,20,18,255},
		.DDT = DLMS_DATA_TYPE_UINT32,
		.read_api = &GET_UINT32_DATA_SYNCHRONIZATION_EVENT_LOGS_FILTER,
		.write_api = &SET_UINT32_DATA_SYNCHRONIZATION_EVENT_LOGS_FILTER,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};

	arr_obj[DATA_PARAMETER_EVENT_LOGS_FILTER] = ( obj_data)
	{
		.obis = {0,0,94,20,19,255},
		.DDT = DLMS_DATA_TYPE_UINT32,
		.read_api = &GET_UINT32_DATA_PARAMETER_EVENT_LOGS_FILTER,
		.write_api = &SET_UINT32_DATA_PARAMETER_EVENT_LOGS_FILTER,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};

	arr_obj[DATA_ACTIVE_IMPORT_TARIFF_SETTNIGS] = ( obj_data)
	{
		.obis = {0,0,94,20,25,255},
		.DDT = DLMS_DATA_TYPE_STRUCTURE,
		.read_api = &GET_STRUCT_DATA_ACTIVE_IMPORT_TARIFF_SETTNIGS,
		.write_api = NULL,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};

	arr_obj[DATA_STANDBY_IMPORT_TARIFF_SETTNIGS] = ( obj_data)
	{
		.obis = {0,0,94,20,26,255},
		.DDT = DLMS_DATA_TYPE_STRUCTURE,
		.read_api = &GET_STRUCT_DATA_STANDBY_IMPORT_TARIFF_SETTNIGS,
		.write_api = &SET_STRUCT_DATA_STANDBY_IMPORT_TARIFF_SETTNIGS,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};

	arr_obj[DATA_ACTIVE_EXPORT_TARIFF_SETTNIGS] = ( obj_data)
	{
		.obis = {0,0,94,20,27,255},
		.DDT = DLMS_DATA_TYPE_STRUCTURE,
		.read_api = &GET_STRUCT_DATA_ACTIVE_EXPORT_TARIFF_SETTNIGS,
		.write_api = NULL,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};

	arr_obj[DATA_STANDBY_EXPORT_TARIFF_SETTNIGS] = ( obj_data)
	{
		.obis = {0,0,94,20,28,255},
		.DDT = DLMS_DATA_TYPE_STRUCTURE,
		.read_api = &GET_STRUCT_DATA_STANDBY_EXPORT_TARIFF_SETTNIGS,
		.write_api = &SET_STRUCT_DATA_STANDBY_EXPORT_TARIFF_SETTNIGS,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};

	arr_obj[DATA_ACTIVE_IMPORT_TARIFF_CODE] = ( obj_data)
	{
		.obis = {0,0,94,20,29,255},
		.DDT = DLMS_DATA_TYPE_UINT16,
		.read_api = &GET_UINT16_DATA_ACTIVE_IMPORT_TARIFF_CODE,
		.write_api = NULL,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};

	arr_obj[DATA_STANDBY_IMPORT_TARIFF_CODE] = ( obj_data)
	{
		.obis = {0,0,94,20,30,255},
		.DDT = DLMS_DATA_TYPE_UINT16,
		.read_api = &GET_UINT16_DATA_STANDBY_IMPORT_TARIFF_CODE,
		.write_api = &SET_UINT16_DATA_STANDBY_IMPORT_TARIFF_CODE,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};

	arr_obj[DATA_ACTIVE_EXPORT_TARIFF_CODE] = ( obj_data)
	{
		.obis = {0,0,94,20,31,255},
		.DDT = DLMS_DATA_TYPE_UINT16,
		.read_api = &GET_UINT16_DATA_ACTIVE_EXPORT_TARIFF_CODE,
		.write_api = NULL,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};

	arr_obj[DATA_STANDBY_EXPORT_TARIFF_CODE] = ( obj_data)
	{
		.obis = {0,0,94,20,32,255},
		.DDT = DLMS_DATA_TYPE_UINT16,
		.read_api = &GET_UINT16_DATA_STANDBY_EXPORT_TARIFF_CODE,
		.write_api = &SET_UINT16_DATA_STANDBY_EXPORT_TARIFF_CODE,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};

	arr_obj[DATA_IMPORT_TARIFF_ACTIVATION_DATE] = ( obj_data)
	{
		.obis = {0,0,94,20,33,255},
		.DDT = DLMS_DATA_TYPE_UINT32,
		.read_api = &GET_UINT32_DATA_IMPORT_TARIFF_ACTIVATION_DATE,
		.write_api = &SET_UINT32_DATA_IMPORT_TARIFF_ACTIVATION_DATE,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};

	arr_obj[DATA_EXPORT_TARIFF_ACTIVATION_DATE] = ( obj_data)
	{
		.obis = {0,0,94,20,34,255},
		.DDT = DLMS_DATA_TYPE_UINT32,
		.read_api = &GET_UINT32_DATA_EXPORT_TARIFF_ACTIVATION_DATE,
		.write_api = &SET_UINT32_DATA_EXPORT_TARIFF_ACTIVATION_DATE,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};

	arr_obj[DATA_CUMULATIVE_RECHARGE_AMOUNT] = ( obj_data)
	{
		.obis = {1,0,96,50,61,255},
		.DDT = DLMS_DATA_TYPE_INT64,
		.read_api = &GET_INT64_DATA_CUMULATIVE_RECHARGE_AMOUNT,
		.write_api = NULL,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};

	arr_obj[DATA_LAST_RECHARGE_AMOUNT] = ( obj_data)
	{
		.obis = {1,0,96,50,84,255},
		.DDT = DLMS_DATA_TYPE_INT32,
		.read_api = &GET_INT32_DATA_LAST_RECHARGE_AMOUNT,
		.write_api = NULL,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};

	arr_obj[DATA_TID] = ( obj_data)
	{
		.obis = {0,0,96,55,5,255},
		.DDT = DLMS_DATA_TYPE_UINT32,
		.read_api = &GET_UINT32_DATA_TID,
		.write_api = NULL,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};
	
	arr_obj[DATA_REMAINING_CREDIT] = (obj_data)
	{
		.obis = {0,0,19,41,0,255},/*todo edit obis*/
		.DDT = DLMS_DATA_TYPE_INT64,
		.read_api = &GET_INT64_DATA_Remaining_credit,
		.write_api = NULL,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};
	
	arr_obj[DATA_CHARGE_TOKEN_GATEWAY] = (obj_data)
	{
		.obis = {0,0,19,40,0,255},
		.DDT = DLMS_DATA_TYPE_UINT64,
		.read_api = &GET_UINT64_DATA_Charge_Token_Gateway,
		.write_api = &SET_UINT64_DATA_Charge_Token_Gateway,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};
	
	arr_obj[DATA_TARIFF_MODE] = (obj_data)
	{
		.obis = {0,0,19,42,0,255},/*todo edit obis*/
		.DDT = DLMS_DATA_TYPE_UINT8,
		.read_api = &GET_UINT8_DATA_Tariff_Mode,
		.write_api = &SET_UINT8_DATA_Tariff_Mode,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};
	
	arr_obj[DATA_TARIFF_CURRENT_STEP] = (obj_data)
	{
		.obis = {0,0,94,20,35,255},/*todo edit obis*/
		.DDT = DLMS_DATA_TYPE_UINT8,
		.read_api = &GET_UINT8_DATA_Tariff_current_step,
		.write_api = NULL,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};
	
	arr_obj[DATA_TAMPER_RESET] = (obj_data)
	{
		.obis = {0,0,94,20,1,255},
		.DDT = DLMS_DATA_TYPE_UINT8,
		.read_api = &GET_UINT8_DATA_Tamper_reset,
		.write_api = &SET_UINT8_DATA_Tamper_reset,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};
	
	arr_obj[DATA_APN] = (obj_data)
	{
		.obis = {0,0,94,20,36,255},
		.DDT = DLMS_DATA_TYPE_STRING,
		.read_api = &GET_STRING_DATA_APN,
		.write_api = &SET_STRING_DATA_APN,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};
	
	arr_obj[DATA_APN_USERNAME] = (obj_data)
	{
		.obis = {0,0,94,20,37,255},
		.DDT = DLMS_DATA_TYPE_STRING,
		.read_api = &GET_STRING_DATA_APN_USERNAME,
		.write_api = NULL,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};
	
	arr_obj[DATA_APN_PASSWORD] = (obj_data)
	{
		.obis = {0,0,94,20,38,255},
		.DDT = DLMS_DATA_TYPE_STRING,
		.read_api = &GET_STRING_DATA_APN_PASSWORD,
		.write_api = NULL,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};

	arr_obj[DATA_WEEKENDS] = (obj_data)
	{
		.obis = {0,0,94,20,42,255},
		.DDT = DLMS_DATA_TYPE_UINT8,
		.read_api = &GET_UINT8_DATA_weekend,
		.write_api = &SET_UINT8_DATA_weekend,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};

	arr_obj[DATA_ADD_HOLIDAY] = (obj_data)
	{
		.obis = {0,0,94,20,43,255},
		.DDT = DLMS_DATA_TYPE_UINT16,
		.read_api = &GET_UINT16_DATA_add_holiday,
		.write_api = &SET_UINT16_DATA_add_holiday,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};

	arr_obj[DATA_REMOVE_HOLIDAY] = (obj_data)
	{
		.obis = {0,0,94,20,44,255},
		.DDT = DLMS_DATA_TYPE_UINT16,
		.read_api = &GET_UINT16_DATA_remove_holiday,
		.write_api = &SET_UINT16_DATA_remove_holiday,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};
	
	arr_obj[DATA_ALL_HOLIDAYS] = (obj_data)
	{
		.obis = {0,0,94,20,45,255},
		.DDT = DLMS_DATA_TYPE_ARRAY,
		.read_api = &GET_ARR_DATA_all_holidays,
		.write_api = &SET_ARR_DATA_all_holidays,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};
	
	arr_obj[DATA_START_FIRENDLY_TIME] = (obj_data)
	{
		.obis = {0,0,94,20,46,255},
		.DDT = DLMS_DATA_TYPE_UINT16,
		.read_api = &GET_UINT16_DATA_start_friendly_time,
		.write_api = &SET_UINT16_DATA_start_friendly_time,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};
	
	arr_obj[DATA_END_FIRENDLY_TIME] = (obj_data)
	{
		.obis = {0,0,94,20,47,255},
		.DDT = DLMS_DATA_TYPE_UINT16,
		.read_api = &GET_UINT16_DATA_end_friendly_time,
		.write_api = &SET_UINT16_DATA_end_friendly_time,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};

	arr_obj[DATA_TEST_MODE] = (obj_data)
	{
		.obis = {0,0,94,20,0,255},
		.DDT = DLMS_DATA_TYPE_STRUCTURE,
		.read_api = &GET_STRUCT_DATA_TEST_MODE,
		.write_api = &SET_STRUCT_DATA_TEST_MODE,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};
	
	arr_obj[DATA_BILLING_HISTORY_INDEX] = (obj_data)
	{
		.obis = {0,0,94,20,51,255},
		.DDT = DLMS_DATA_TYPE_UINT16,
		.read_api = &GET_UINT16_DATA_BILLING_HISTORY_INDEX,
		.write_api = &SET_UINT16_DATA_BILLING_HISTORY_INDEX,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};
	
	arr_obj[DATA_BILLING_HISTORY] = (obj_data)
	{
		.obis = {0,0,94,20,52,255},
		.DDT = DLMS_DATA_TYPE_STRUCTURE,
		.read_api = &GET_STUCT_DATA_BILLING_HISTORY,
		.write_api = NULL,
		.access_mode = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE}
	};
	return 1;
}

int init_all_data(gxData AllData[]){
	obj_data arr_obj[max_no_of_data];
	_init_arra_data(arr_obj);
	int ret = 0;
	uint8_t default_access[4] = {DLMS_ACCESS_MODE_NONE,DLMS_ACCESS_MODE_READ,DLMS_ACCESS_MODE_READ_WRITE,DLMS_ACCESS_MODE_NONE};
	
	for (int Obj_index = 0; Obj_index < max_no_of_data; Obj_index++)
	{
		gxData globalData;
		const unsigned char* ln = arr_obj[Obj_index].obis;
		ret = cosem_init2((gxObject *)&globalData, DLMS_OBJECT_TYPE_DATA, ln);
		globalData.value.vt = arr_obj[Obj_index].DDT;
		globalData.read_api=arr_obj[Obj_index].read_api;
		globalData.write_api=arr_obj[Obj_index].write_api;
		if (globalData.base.access == NULL)
		{
			globalData.base.access = (gxAccess*)gxmalloc(sizeof(gxAccess));
		}
		if (globalData.base.access != NULL)
		{
			BYTE_BUFFER_INIT(&globalData.base.access->attributeAccessModes);
			
			if(arr_obj[Obj_index].access_mode != 0x0){
				
				ret &=bb_insert(default_access,4,&globalData.base.access->attributeAccessModes,0);
			}
			else{
				ret &=bb_insert(&arr_obj[Obj_index].access_mode,4,&globalData.base.access->attributeAccessModes,0);
				
			}
			if (ret != 0)
			{
				break;
			}
			}else{
			break;
		}
		AllData[Obj_index] = globalData;
	}

	
	return ret;
};

DLMS_ERROR_CODE handle_data_write_callback(gxData* AllData,unsigned char* logicalName ,char index ,dlmsVARIANT*  byteArr){
	int len=0;
	DLMS_ERROR_CODE ret = 0;
	for (int rg = 0; rg < max_no_of_data; rg++)
	{
		
		int cpresult = memcmp(logicalName, AllData[rg].base.logicalName, 6);
		
		if (cpresult == 0 && AllData[rg].write_api != NULL)
		{
			if(index ==2) /* handle value attribute*/
			{
				switch (AllData[rg].value.vt)
				{
					/*case DLMS_DATA_TYPE_OCTET_STRING:
					{
					int return_v =  cosem_setOctetString2(byteArr,AllData[rg].api(&len),len);
					return return_v;
					}
					 					case DLMS_DATA_TYPE_BIT_STRING:
					 					{
					 						ret = AllData[rg].write_api(&byteArr->puiVal,&len);
					 						return ret;
					 					}
										break;*/
					case DLMS_DATA_TYPE_STRING:
					
					len = byteArr->Arr->size;
					ret = AllData[rg].write_api(byteArr->Arr->data,&len);
					
					return ret;
					break;
					case DLMS_DATA_TYPE_ARRAY:
					{
						len = byteArr->Arr->size;
						ret = AllData[rg].write_api(byteArr->Arr->data,&len);
						return ret;
					}
					break;
					case DLMS_DATA_TYPE_STRUCTURE:
					{
						len = byteArr->Arr->size;
						ret = AllData[rg].write_api(byteArr->Arr->data,&len);
						return ret;
					}
					break;
					
					case DLMS_DATA_TYPE_UINT64:
					{
						len=8;
						ret = AllData[rg].write_api(&byteArr->puiVal,&len);
						return ret;
					}
					break;
					case DLMS_DATA_TYPE_UINT32:
					{
						len=4;
						ret = AllData[rg].write_api(&byteArr->puiVal,&len);
						return ret;
					}
					break;
					case DLMS_DATA_TYPE_UINT16:
					{
						len=2;
						ret = AllData[rg].write_api(&byteArr->puiVal,&len);
						return  ret;
					}
					break;
					case DLMS_DATA_TYPE_UINT8:
					{
						len=1;
						ret = AllData[rg].write_api(&byteArr->puiVal,&len);
						return  ret;
					}
					break;
					default :
					
					break;
					
				}/*/end of switch*/
				
			}/*end of attr2*/
			return 0; /* i found what i need*/
		} /*end of if*/
		
	} /*loop for all obj*/
	return DLMS_ERROR_CODE_OK;
}
DLMS_ERROR_CODE handle_data_read_callback(gxData* AllData,unsigned char * logicalName ,char index ,gxByteBuffer*  byteArr,DLMS_DATA_TYPE* vt){
	int len=0;
	for (int rg = 0; rg < max_no_of_data; rg++)
	{
		
		int cpresult = memcmp(logicalName, AllData[rg].base.logicalName, 6);
		
		if (cpresult == 0)
		{
			if(index ==2) /* handle value attribute*/
			{
				*vt = AllData[rg].value.vt;
				switch (AllData[rg].value.vt)
				{
					case DLMS_DATA_TYPE_OCTET_STRING:
					{
						int return_v =  cosem_setOctetString2(byteArr,AllData[rg].read_api(&len),len);
						return return_v;
					}
					break;
					case DLMS_DATA_TYPE_BIT_STRING:
					{
						int return_v =  cosem_setBitString(byteArr,*(uint32_t*)AllData[rg].read_api(&len),len);
						return return_v;
					}
					break;
					case DLMS_DATA_TYPE_STRING:
					{
						int return_v =  cosem_setString(byteArr,AllData[rg].read_api(&len),len);
						return return_v;
					}
					break;
					case DLMS_DATA_TYPE_ARRAY:
					{
						int returnv = 0;
						gxByteBuffer* returneddata = AllData[rg].read_api(&len);
						returnv =  bb_set2(byteArr,returneddata,byteArr->position ,len);
						bb_clear(returneddata);
						return returnv;
					}
					break;
					case DLMS_DATA_TYPE_STRUCTURE:
					{
						int returnv = 0;
						gxByteBuffer* returneddata = AllData[rg].read_api(&len);
						returnv =  bb_set2(byteArr,returneddata,byteArr->position ,len);
						bb_clear(returneddata);
						return returnv;
					}
					break;
					case DLMS_DATA_TYPE_INT64:
					{
						int return_v =  cosem_setInt64(byteArr,(int64_t *)AllData[rg].read_api(&len));
						return return_v;
					}
					break;
					case DLMS_DATA_TYPE_UINT64:
					{
						int return_v =  cosem_setUInt64(byteArr,(uint64_t *)AllData[rg].read_api(&len));
						return return_v;
					}
					break;
					case DLMS_DATA_TYPE_INT32:
					{
						int return_v =cosem_setInt32(byteArr,*(int32_t *) AllData[rg].read_api(&len));
						return return_v;
					}
					break;
					case DLMS_DATA_TYPE_UINT32:
					{
						int return_v =cosem_setUInt32(byteArr,*(uint32_t *) AllData[rg].read_api(&len));
						return return_v;
					}
					break;
					case DLMS_DATA_TYPE_UINT16:
					{
						
						return  cosem_setUInt16(byteArr,*(uint16_t *)AllData[rg].read_api(&len));
					}
					break;
					case DLMS_DATA_TYPE_UINT8:
					{
						return cosem_setUInt8(byteArr, *(uint8_t *)AllData[rg].read_api(&len));
					}
					break;
					default :
					
					break;
					
				}/*/end of switch*/
				
			}/*end of attr2*/
			return (0); /* i found what i need*/
		} /*end of if*/
		
	} /*loop for all obj*/
	return (0);
}/*fun*/

int Data_HandleMethodCallBack(char index)
{
	int ret = 0;
	switch (index)
	{
		case 1: /* adjust_to_quarter (integer data)*/
		/* Todo Add Requested Action*/
		
		break;
		default:
		break;
	}
}
