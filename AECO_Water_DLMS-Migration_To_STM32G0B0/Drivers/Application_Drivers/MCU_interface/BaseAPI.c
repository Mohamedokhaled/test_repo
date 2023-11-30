/*
* BaseAPI.c
*
* Created: 3/28/2023 7:28:41 PM
*  Author: asamir
*/
#include "BaseAPI.h"
#include "GSM.h"
#include "dlms_api.h"
#include "solenoid.h"
#include "billing.h"
#include "display.h"
#include "EEPROM.h"


/* used to round currency to nearest 0.01 and multiply by 100*/
#define ROUND_2_DEC(var) ( ( (int64_t)( (var)*1000) )%10 >=5 || ( (int64_t)( (var)*1000) )%10 <= -5)? 		\
						 ( ( (int64_t) ( ( (var) * 1000) + (((var)>0)?10:-10) ) ) / 10 ): 					\
						 ( ((int64_t)((var) * 1000)) / 10 )

extern status_t dataStatus;
extern config_t dataConfig;


test_mode_status_t test_mode_status;

/* TODO:: handle corner case when saving dynamic sized objects */

/* please add new apis at the bottom */
/**
 * @brief Get the int invocation counter object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_INT_INVOCATION_COUNTER(int* len)
{
	*len =4;
	static uint32_t invocation=0;
	invocation=  dlms_HdlcInstance.base.cipher.invocationCounter;
/* 	 invocation = 200; */
	return &invocation;
}
/**
 * @brief Get the int meter serial object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_INT_METER_SERIAL(int* len)
{
	*len =4;
	static uint32_t serial=987654321;
	return &serial;
}
/**
 * @brief Get the int device id object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_INT_DEVICE_ID(int* len){
	*len =4;
	static uint32_t id=123123123;
	return &id;
}
/**
 * @brief Get the octet block cipher key object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_OCTET_BLOCK_CIPHER_KEY(int* len){
	*len =16;
	static uint8_t blockCipherKey[16] = {0};
	 uint8_t buf[32] = {0};
	/** TODO:: secure element handle**/
	/* read_guek(buf); */
	memcpy(blockCipherKey,&buf[16],16);
	
	return &blockCipherKey;
}
/**
 * @brief Get the octet authentication key object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_OCTET_AUTHENTICATION_KEY(int* len){
	*len =16;
	 uint8_t buf[32]={0};
	static uint8_t authKey[16] ={0};
	/** TODO:: secure element handle**/
	/* read_ak(buf); */
	memcpy(authKey,&buf[16],16);
	
	return &authKey;
}
/**
 * @brief Get the octet key of keys object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_OCTET_KEY_OF_KEYS(int* len){
	*len =16;
	static unsigned char KEK[16] = { 0 }; 
	 uint8_t buf[32]={0};
	 	/** TODO:: secure element handle**/
		 	 /* read_kek(buf); */
	memcpy(KEK,&buf[16],16);
	 
	return &KEK;
}

/**
 * @brief 
 * 
 * @param data 
 * @param len 
 * @return DLMS_ERROR_CODE 
 */
DLMS_ERROR_CODE SET_Meter_First_Setup(unsigned char* data,int* len){

	/** TODO:: external flash handle **/
	/* ext_flash_EraseFullChip(); */

	return DLMS_ERROR_CODE_OK;
}

/**
 * @brief Get the octet data customer id object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_OCTET_DATA_Customer_ID(int* len)
{ 
	*len =4;
	extern billing_totalsRegisters billing_customerTotalsRegisters;
	return &billing_customerTotalsRegisters.ID;
}
/**
 * @brief Set the octet data customer id object
 * 
 * @param data 
 * @param len 
 * @return DLMS_ERROR_CODE 
 */
DLMS_ERROR_CODE SET_OCTET_DATA_Customer_ID(unsigned char* data,int* len)
{

	billing_ResetForNewCustomer(*(uint32_t*)data);
	
	return DLMS_ERROR_CODE_OK;
}

/**
 * @brief Get the octet data meter installement date object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_OCTET_DATA_Meter_Installement_Date(int* len)
{
    *len =4;
	static uint32_t id= 12;
	return &id;
}
/**
 * @brief Set the octet data meter installement date object
 * 
 * @param data 
 * @param len 
 * @return DLMS_ERROR_CODE 
 */
DLMS_ERROR_CODE SET_OCTET_DATA_Meter_Installement_Date(unsigned char* data,int* len)
{
    return DLMS_ERROR_CODE_OK;
}
/**
 * @brief Get the uint32 data configure meter status parameters object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_UINT32_DATA_Configure_Meter_Status_Parameters(int* len)
{
	*len=4;
	return &dataConfig.data;
}
/**
 * @brief Set the uint32 data configure meter status parameters object
 * 
 * @param data 
 * @param len 
 * @return DLMS_ERROR_CODE 
 */
DLMS_ERROR_CODE SET_UINT32_DATA_Configure_Meter_Status_Parameters(unsigned char* data,int* len)
{
    dataConfig.data = *(uint32_t*)data;

    return DLMS_ERROR_CODE_OK;
}
/**
 * @brief Get the struct data display items object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_STRUCT_DATA_DISPLAY_ITEMS(int* len)
{
	static gxByteBuffer tmp ;
	cosem_setStructure(&tmp,2);
	cosem_setUInt8(&tmp,lcd_config.screen_disp - 1); 						/* return the screens number*/
	
	if (lcd_config.screen_disp != 0) 
	{
		cosem_setStructure(&tmp,5);
		cosem_setArray(&tmp,6); 												/* logical name*/
		for (uint8_t logical_name_index = 0; logical_name_index < 6; logical_name_index++)
		{
			cosem_setUInt8(&tmp,lcd_config.screen[lcd_config.screen_disp - 1].logical_name[logical_name_index]);
		}
		cosem_setUInt32(&tmp,lcd_config.screen[lcd_config.screen_disp - 1].classID);
		cosem_setUInt8(&tmp,lcd_config.screen[lcd_config.screen_disp - 1].attribute_id);
		cosem_setUInt8(&tmp,lcd_config.screen[lcd_config.screen_disp - 1].numOfDigitAfterDot);
		cosem_setUInt8(&tmp,lcd_config.screen[lcd_config.screen_disp - 1].numOfDigitBeforeDot);
	}
	else /*   if the screen is not displaying anything return empty structure */
	{
		cosem_setStructure(&tmp,5);
		cosem_setArray(&tmp,6); /*   logical name */
		for (uint8_t logical_name_index = 0; logical_name_index < 6; logical_name_index++)
		{
			cosem_setUInt8(&tmp,lcd_config.screen[0].logical_name[logical_name_index]);
		}
		cosem_setUInt32(&tmp,lcd_config.screen[0].classID);
		cosem_setUInt8(&tmp,lcd_config.screen[0].attribute_id);
		cosem_setUInt8(&tmp,lcd_config.screen[0].numOfDigitAfterDot);
		cosem_setUInt8(&tmp,lcd_config.screen[0].numOfDigitBeforeDot);
	}
	*len = tmp.size;
	return &tmp;
	
}

/**
 * @brief Set the struct data display items object
 * 
 * @param data 
 * @param len 
 * @return DLMS_ERROR_CODE 
 */
DLMS_ERROR_CODE SET_STRUCT_DATA_DISPLAY_ITEMS(unsigned char* data,int* len)
{
	uint8_t display_request = 0;

	dlmsVARIANT** screen_struct = (dlmsVARIANT**)(((dlmsVARIANT**)data)[1]->Arr->data);
	dlmsVARIANT** logical_name_array = screen_struct[0]->Arr->data;
	
	if (*len == 2)
	/*   check the data format before assigning any thing */
	{
		if (((dlmsVARIANT**)data)[0]->vt != DLMS_DATA_TYPE_UINT8) /*   screen on period is not in the right format */
		{
			return DLMS_ERROR_CODE_INVALID_DATA_FORMAT;
		}
		if (((dlmsVARIANT**)data)[1]->vt != DLMS_DATA_TYPE_STRUCTURE) /*   screen display num is not in the right format */
		{
			return DLMS_ERROR_CODE_INVALID_DATA_FORMAT;
		}
	}
	else
	{
		return DLMS_ERROR_CODE_INVALID_DATA_FORMAT;
	}	
	/*   right format */

	var_getInt8(((dlmsVARIANT**)data)[0],&display_request); /*   according to the request data will be handled */
	
	if(display_request <= lcd_config.screen_disp)
	{
		for (int i =0;i<6;i++)
		{
			var_getInt8(logical_name_array[i],&lcd_config.screen[display_request].logical_name[i]);
		}
		var_getInt32(screen_struct[1],&lcd_config.screen[display_request].classID);
		var_getInt8(screen_struct[2],&lcd_config.screen[display_request].attribute_id);
		var_getInt8(screen_struct[3],&lcd_config.screen[display_request].numOfDigitAfterDot);
		var_getInt8(screen_struct[4],&lcd_config.screen[display_request].numOfDigitBeforeDot);
		if (display_request == lcd_config.screen_disp)
		{
			lcd_config.screen_disp++;
		}
	}
	else/*    error  */
	{
		return DLMS_ERROR_CODE_INVALID_COMMAND;
	}
    return DLMS_ERROR_CODE_OK;
}

/**
 * @brief Get the struct data general display settings object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_STRUCT_DATA_GENERAL_DISPLAY_SETTINGS(int* len)
{

	static gxByteBuffer tmp ;

	cosem_setStructure(&tmp,3);
	cosem_setUInt8(&tmp,  lcd_config.screenPeriod);
	cosem_setUInt8(&tmp,  lcd_config.LcdOffPeriod);
	cosem_setUInt8(&tmp,  lcd_config.screen_disp);

	*len = tmp.size;
	return &tmp;

}

/* init screen function add new screen*/
/**
 * @brief Set the struct data general display settings object
 * 
 * @param data 
 * @param len 
 * @return DLMS_ERROR_CODE 
 */
DLMS_ERROR_CODE SET_STRUCT_DATA_GENERAL_DISPLAY_SETTINGS(unsigned char* data,int* len)
{

	if (*len == 3 )
	/*   check the data format before assigning any thing */
	{
		if (((dlmsVARIANT**)data)[0]->vt != DLMS_DATA_TYPE_UINT8) /*   screen on period is not in the right format */
		{
			return DLMS_ERROR_CODE_INVALID_DATA_FORMAT;
		}
		
		if (((dlmsVARIANT**)data)[1]->vt != DLMS_DATA_TYPE_UINT8) /*   screen off period is not in the right format */
		{
			return DLMS_ERROR_CODE_INVALID_DATA_FORMAT;
		}
		if (((dlmsVARIANT**)data)[2]->vt != DLMS_DATA_TYPE_UINT8) /*   screen display num is not in the right format */
		{
			return DLMS_ERROR_CODE_INVALID_DATA_FORMAT;
		}
	}
	else   if (*len == 3 )
	{
		return  DLMS_ERROR_CODE_INVALID_DATA_FORMAT;
	} 
	
	var_getUInt8(((dlmsVARIANT**)data)[0],&lcd_config.screenPeriod);/*  screen on period 4 sec */
	var_getUInt8(((dlmsVARIANT**)data)[1],&lcd_config.LcdOffPeriod);/*   screen off period 5 min (sleep mode) */
	var_getUInt8(((dlmsVARIANT**)data)[2],&lcd_config.screen_disp);/*   number of screens */

	
	return DLMS_ERROR_CODE_OK;
}

/**
 * @brief Get the uint32 data meter status object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_UINT32_DATA_METER_STATUS(int* len)
{
	*len = 4;
	return &dataStatus.data;
}

/**
 * @brief Get the uint32 data recharge count object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_UINT32_DATA_RECHARGE_COUNT(int* len)
{
    *len =4;
	return &_event[EVENT_NAME_RECHRAGE].count;
}
/**
 * @brief Get the uint32 data meter tripping count object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_UINT32_DATA_METER_TRIPPING_COUNT(int* len)
{
    *len =4;
	return &_event[EVENT_NAME_METER_TRIPPING].count;
}
/**
 * @brief Get the uint32 data parameter update count object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_UINT32_DATA_PARAMETER_UPDATE_COUNT(int* len)
{
    *len =4;
	return &_event[EVENT_NAME_PARAMETER_UPDATE].count;
}
/**
 * @brief Get the uint32 data adjusting clock count object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_UINT32_DATA_ADJUSTING_CLOCK_COUNT(int* len)
{
    *len =4;
	return &_event[EVENT_NAME_ADJUSTING_CLOCK].count;
}
/**
 * @brief Get the uint32 data billing reset count object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_UINT32_DATA_BILLING_RESET_COUNT(int* len)
{
    *len =4;
	return &_event[EVENT_NAME_BILLING_RESET].count;
}
/**
 * @brief Get the uint32 data active firmware identifier object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_UINT32_DATA_ACTIVE_FIRMWARE_IDENTIFIER(int* len)
{
	*len = 4;
	/** TODO:: move it the display module**/
#define SoftWare_Version   1100123U
	static uint32_t fw_version = SoftWare_Version;
	return &fw_version;
}
/**
 * @brief Get the octet data active firmware signature object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_OCTET_DATA_ACTIVE_FIRMWARE_SIGNATURE(int* len)
{
    *len =4;
	static uint32_t id= 48;
	return &id;
}
/**
 * @brief Get the uint32 data common event logs filter object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_UINT32_DATA_COMMON_EVENT_LOGS_FILTER(int* len)
{
    *len =4;
	static uint32_t id= 49;
	return &id;
}
/**
 * @brief Set the uint32 data common event logs filter object
 * 
 * @param data 
 * @param len 
 * @return DLMS_ERROR_CODE 
 */
DLMS_ERROR_CODE SET_UINT32_DATA_COMMON_EVENT_LOGS_FILTER(unsigned char* data,int* len)
{
    return DLMS_ERROR_CODE_OK;
}

/**
 * @brief Get the uint32 data fraud event logs filter object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_UINT32_DATA_FRAUD_EVENT_LOGS_FILTER(int* len)
{
    *len =4;
	static uint32_t id= 50;
	return &id;
}
/**
 * @brief Set the uint32 data fraud event logs filter object
 * 
 * @param data 
 * @param len 
 * @return DLMS_ERROR_CODE 
 */
DLMS_ERROR_CODE SET_UINT32_DATA_FRAUD_EVENT_LOGS_FILTER(unsigned char* data,int* len)
{
    return DLMS_ERROR_CODE_OK;
}

/**
 * @brief Get the uint32 data firmware event logs filter object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_UINT32_DATA_FIRMWARE_EVENT_LOGS_FILTER(int* len)
{
    *len =4;
	static uint32_t id= 51;
	return &id;
}
/**
 * @brief Set the uint32 data firmware event logs filter object
 * 
 * @param data 
 * @param len 
 * @return DLMS_ERROR_CODE 
 */
DLMS_ERROR_CODE SET_UINT32_DATA_FIRMWARE_EVENT_LOGS_FILTER(unsigned char* data,int* len)
{
    return DLMS_ERROR_CODE_OK;
}


unsigned char* GET_UINT32_DATA_SYNCHRONIZATION_EVENT_LOGS_FILTER(int* len)
{
    *len =4;
	static uint32_t id= 52;
	return &id;
}
/**
 * @brief Set the uint32 data synchronization event logs filter object
 * 
 * @param data 
 * @param len 
 * @return DLMS_ERROR_CODE 
 */
DLMS_ERROR_CODE SET_UINT32_DATA_SYNCHRONIZATION_EVENT_LOGS_FILTER(unsigned char* data,int* len)
{
    return DLMS_ERROR_CODE_OK;
}

/**
 * @brief Get the uint32 data parameter event logs filter object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_UINT32_DATA_PARAMETER_EVENT_LOGS_FILTER(int* len)
{
    *len =4;
	static uint32_t id= 53;
	return &id;
}
/**
 * @brief Set the uint32 data parameter event logs filter object
 * 
 * @param data 
 * @param len 
 * @return DLMS_ERROR_CODE 
 */
DLMS_ERROR_CODE SET_UINT32_DATA_PARAMETER_EVENT_LOGS_FILTER(unsigned char* data,int* len)
{
    return DLMS_ERROR_CODE_OK;
}

/**
 * @brief Get the struct data active import tariff settnigs object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_STRUCT_DATA_ACTIVE_IMPORT_TARIFF_SETTNIGS(int* len)
{
	static gxByteBuffer active_import_tariff_bytebuffer = {0};
	extern billing_stepTariffPlan billing_currentStepTariffPlan;
	cosem_setStructure(&active_import_tariff_bytebuffer ,5);
	cosem_setUInt8(&active_import_tariff_bytebuffer ,billing_currentStepTariffPlan.activationMonth);
	cosem_setUInt16(&active_import_tariff_bytebuffer ,billing_currentStepTariffPlan.activationYear);
	cosem_setUInt8(&active_import_tariff_bytebuffer ,billing_currentStepTariffPlan.stepFeeDeductionRule);	
	cosem_setInt16(&active_import_tariff_bytebuffer ,(billing_currentStepTariffPlan.zeroConsumptionFee));
	cosem_setArray(&active_import_tariff_bytebuffer, NO_OF_TARIFF_STEPS);
	
	for (int i = 0; i < NO_OF_TARIFF_STEPS; i++)
	{
		cosem_setStructure(&active_import_tariff_bytebuffer,5);
		cosem_setUInt8(&active_import_tariff_bytebuffer,billing_currentStepTariffPlan.steps[i].stepNo);
 		cosem_setInt16(&active_import_tariff_bytebuffer,(uint32_t)(billing_currentStepTariffPlan.steps[i].stepPrice));
		cosem_setInt16(&active_import_tariff_bytebuffer,(uint32_t)(billing_currentStepTariffPlan.steps[i].stepFee));
		cosem_setUInt8(&active_import_tariff_bytebuffer,billing_currentStepTariffPlan.steps[i].recalculationFlag);
		cosem_setUInt16(&active_import_tariff_bytebuffer,billing_currentStepTariffPlan.steps[i].toKWH);
	}
	
	*len = active_import_tariff_bytebuffer.size;
	return &active_import_tariff_bytebuffer;
}

/**
 * @brief Get the struct data standby import tariff settnigs object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_STRUCT_DATA_STANDBY_IMPORT_TARIFF_SETTNIGS(int* len)
{	
	static gxByteBuffer standby_import_tariff_bytebuffer = {0};
	extern billing_stepTariffPlan billing_futureStepTariffPlan;
	cosem_setStructure(&standby_import_tariff_bytebuffer ,5);
	cosem_setUInt8(&standby_import_tariff_bytebuffer ,billing_futureStepTariffPlan.activationMonth);
	cosem_setUInt16(&standby_import_tariff_bytebuffer ,billing_futureStepTariffPlan.activationYear);
	cosem_setUInt8(&standby_import_tariff_bytebuffer ,billing_futureStepTariffPlan.stepFeeDeductionRule);
	cosem_setInt16(&standby_import_tariff_bytebuffer ,(billing_futureStepTariffPlan.zeroConsumptionFee));
	cosem_setArray(&standby_import_tariff_bytebuffer, NO_OF_TARIFF_STEPS);
	
	for (int i=0;i < NO_OF_TARIFF_STEPS; i++)
	{
		cosem_setStructure(&standby_import_tariff_bytebuffer ,5);
/*  		cosem_setUInt8(&standby_import_tariff_bytebuffer ,i); */
 		cosem_setUInt8(&standby_import_tariff_bytebuffer ,billing_futureStepTariffPlan.steps[i].stepNo);
 		cosem_setInt16(&standby_import_tariff_bytebuffer ,(uint32_t)(billing_futureStepTariffPlan.steps[i].stepPrice));
 		cosem_setInt16(&standby_import_tariff_bytebuffer ,(uint32_t)(billing_futureStepTariffPlan.steps[i].stepFee));
		cosem_setUInt8(&standby_import_tariff_bytebuffer ,billing_futureStepTariffPlan.steps[i].recalculationFlag);
		cosem_setUInt16(&standby_import_tariff_bytebuffer ,billing_futureStepTariffPlan.steps[i].toKWH);
	}	
	*len = standby_import_tariff_bytebuffer.size;
	return &standby_import_tariff_bytebuffer ;
}
/**
 * @brief Set the struct data standby import tariff settnigs object
 * 
 * @param data 
 * @param len 
 * @return DLMS_ERROR_CODE 
 */
DLMS_ERROR_CODE SET_STRUCT_DATA_STANDBY_IMPORT_TARIFF_SETTNIGS(unsigned char* data,int* len)
{

 	extern billing_stepTariffPlan billing_futureStepTariffPlan;
	dlmsVARIANT** struct_data = (dlmsVARIANT**)data;
	variantArray* steps_array = struct_data[4]->Arr;
	int32_t tmp; /*   to convert the data sent as int32 to float and save it to the right location */
/*   	if(*len != 5)
  	{
  		return DLMS_ERROR_CODE_INVALID_DATA_FORMAT;
  	} */
	DLMS_ERROR_CODE error = DLMS_ERROR_CODE_OK;
	var_getUInt8(struct_data[2],&billing_futureStepTariffPlan.activationMonth);
	var_getUInt16(struct_data[3],&billing_futureStepTariffPlan.activationYear);
	var_getUInt8(struct_data[1],&billing_futureStepTariffPlan.stepFeeDeductionRule);
	var_getInt16(struct_data[0],&billing_futureStepTariffPlan.zeroConsumptionFee);
	
	for (int steps_NO = 0; steps_NO < steps_array->size; steps_NO++)
	{
		dlmsVARIANT** array_Of_Ptr_To_Step_Data = ((dlmsVARIANT*)steps_array->data[steps_NO])->Arr->data;
		/*assign values to billing_futureStepTariffPlan steps*/
		var_getUInt8(array_Of_Ptr_To_Step_Data[0],&billing_futureStepTariffPlan.steps[steps_NO].stepNo);
		var_getInt16(array_Of_Ptr_To_Step_Data[1],&billing_futureStepTariffPlan.steps[steps_NO].stepPrice);
		var_getInt16(array_Of_Ptr_To_Step_Data[2],&billing_futureStepTariffPlan.steps[steps_NO].stepFee);
		var_getUInt8(array_Of_Ptr_To_Step_Data[3],&billing_futureStepTariffPlan.steps[steps_NO].recalculationFlag);
		var_getUInt16(array_Of_Ptr_To_Step_Data[4],&billing_futureStepTariffPlan.steps[steps_NO].toKWH);
	}
    return error;
}
/**
 * @brief Get the struct data active export tariff settnigs object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_STRUCT_DATA_ACTIVE_EXPORT_TARIFF_SETTNIGS(int* len)
{
	static gxByteBuffer active_export_tariff_bytebuffer = {0};
	extern billing_stepTariffPlan billing_currentStepTariffPlan;
	cosem_setStructure(&active_export_tariff_bytebuffer ,5);
	cosem_setUInt8(&active_export_tariff_bytebuffer ,billing_currentStepTariffPlan.activationMonth);
	cosem_setUInt16(&active_export_tariff_bytebuffer ,billing_currentStepTariffPlan.activationYear);
	cosem_setUInt8(&active_export_tariff_bytebuffer ,billing_currentStepTariffPlan.stepFeeDeductionRule);
	cosem_setInt16(&active_export_tariff_bytebuffer ,(billing_currentStepTariffPlan.zeroConsumptionFee));
	cosem_setArray(&active_export_tariff_bytebuffer ,NO_OF_TARIFF_STEPS);
	
	for (int i=0;i < NO_OF_TARIFF_STEPS; i++)
	{
		cosem_setStructure(&active_export_tariff_bytebuffer ,5);
		cosem_setUInt8(&active_export_tariff_bytebuffer,i);
		cosem_setInt32(&active_export_tariff_bytebuffer ,(uint32_t)(0*100));
		cosem_setInt32(&active_export_tariff_bytebuffer ,(uint32_t)(0*100));
		cosem_setUInt8(&active_export_tariff_bytebuffer ,0);
		cosem_setUInt16(&active_export_tariff_bytebuffer ,0);
	}	
	*len = active_export_tariff_bytebuffer.size;
	return &active_export_tariff_bytebuffer ;
}
/**
 * @brief Get the struct data standby export tariff settnigs object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_STRUCT_DATA_STANDBY_EXPORT_TARIFF_SETTNIGS(int* len)
{
	extern billing_stepTariffPlan billing_futureStepTariffPlan;
	static gxByteBuffer standby_export_tariff_bytebuffer = {0};
	cosem_setStructure(&standby_export_tariff_bytebuffer ,5);
	cosem_setUInt8(&standby_export_tariff_bytebuffer ,billing_futureStepTariffPlan.activationMonth);
	cosem_setUInt16(&standby_export_tariff_bytebuffer ,billing_futureStepTariffPlan.activationYear);
	cosem_setUInt8(&standby_export_tariff_bytebuffer ,billing_futureStepTariffPlan.stepFeeDeductionRule);
	cosem_setInt16(&standby_export_tariff_bytebuffer ,(billing_futureStepTariffPlan.zeroConsumptionFee));
	cosem_setArray(&standby_export_tariff_bytebuffer,NO_OF_TARIFF_STEPS);
	
	for (int i = 0;i < NO_OF_TARIFF_STEPS; i++)
	{
		cosem_setStructure(&standby_export_tariff_bytebuffer,5);
		cosem_setUInt8(&standby_export_tariff_bytebuffer,i);
		cosem_setInt32(&standby_export_tariff_bytebuffer,(uint32_t)(0*100));
		cosem_setInt32(&standby_export_tariff_bytebuffer,(uint32_t)(0*100));
		cosem_setUInt8(&standby_export_tariff_bytebuffer,0);
		cosem_setUInt16(&standby_export_tariff_bytebuffer,0);
	}	
	*len = standby_export_tariff_bytebuffer.size;
	return &standby_export_tariff_bytebuffer;
}
/**
 * @brief Set the struct data standby export tariff settnigs object
 * 
 * @param data 
 * @param len 
 * @return DLMS_ERROR_CODE 
 */
DLMS_ERROR_CODE SET_STRUCT_DATA_STANDBY_EXPORT_TARIFF_SETTNIGS(unsigned char* data,int* len)
{

	static gxByteBuffer standby_export_tariff_bytebuffer = {0};
	extern billing_stepTariffPlan billing_futureStepTariffPlan;
	int ret = cosem_checkArray(data,standby_export_tariff_bytebuffer.size);
	if(ret != DLMS_ERROR_CODE_OK)
	{
		return ret;
	}
	
	for (int i =0;i < NO_OF_TARIFF_STEPS;i++)
	{
		/* todo assign values to billing_futureStepTariffPlan */
		/*  		billing_futureStepTariffPlan.steps[i].stepNo = ((tariffStep *)data)[i].stepNo;
		  		billing_futureStepTariffPlan.steps[i].stepPrice = ((tariffStep *)data)[i].stepPrice/100;
		  		billing_futureStepTariffPlan.steps[i].stepFee = ((tariffStep *)data)[i].stepFee/100;
		  		billing_futureStepTariffPlan.steps[i].recalculationFlag = ((tariffStep *)data)[i].recalculationFlag;
		  		billing_futureStepTariffPlan.steps[i].toKWH = ((tariffStep *)data)[i].toKWH; */
	}
    return DLMS_ERROR_CODE_OK;
}
/**
 * @brief Get the uint16 data active import tariff code object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_UINT16_DATA_ACTIVE_IMPORT_TARIFF_CODE(int* len)
{
    *len =2;
	extern billing_stepTariffPlan billing_currentStepTariffPlan;
	return &billing_currentStepTariffPlan.activationCode;
}
/**
 * @brief Get the uint16 data standby import tariff code object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_UINT16_DATA_STANDBY_IMPORT_TARIFF_CODE(int* len)
{
    *len =2;
	extern billing_stepTariffPlan billing_futureStepTariffPlan;
	return &billing_futureStepTariffPlan.activationCode;
}
/**
 * @brief Set the uint16 data standby import tariff code object
 * 
 * @param data 
 * @param len 
 * @return DLMS_ERROR_CODE 
 */
DLMS_ERROR_CODE SET_UINT16_DATA_STANDBY_IMPORT_TARIFF_CODE(unsigned char* data,int* len)
{
	extern billing_stepTariffPlan billing_currentStepTariffPlan;
	extern billing_stepTariffPlan billing_futureStepTariffPlan;
	if (*(uint16_t*)data <= billing_currentStepTariffPlan.activationCode)
	{
		return DLMS_ERROR_CODE_SERIALIZATION_COUNT_FAILURE;
	}
	billing_futureStepTariffPlan.activationCode = *(uint16_t*)data;
    return DLMS_ERROR_CODE_OK;
}

/**
 * @brief Get the uint16 data active export tariff code object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_UINT16_DATA_ACTIVE_EXPORT_TARIFF_CODE(int* len)
{
    *len =2;
	static uint16_t id= 60;
	return &id;
}
/**
 * @brief Get the uint16 data standby export tariff code object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_UINT16_DATA_STANDBY_EXPORT_TARIFF_CODE(int* len)
{
    *len =2;
	static uint16_t id= 61;
	return &id;
}
/**
 * @brief Set the uint16 data standby export tariff code object
 * 
 * @param data 
 * @param len 
 * @return DLMS_ERROR_CODE 
 */
DLMS_ERROR_CODE SET_UINT16_DATA_STANDBY_EXPORT_TARIFF_CODE(unsigned char* data,int* len)
{
    return DLMS_ERROR_CODE_OK;
}
/**
 * @brief Get the uint32 data import tariff activation date object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_UINT32_DATA_IMPORT_TARIFF_ACTIVATION_DATE(int* len)
{
    *len = 4;
	extern billing_stepTariffPlan billing_futureStepTariffPlan;
	static uint32_t activation_date = 0 ;
	activation_date = billing_futureStepTariffPlan.activationMonth *10000 
						+ billing_futureStepTariffPlan.activationYear;
	return &activation_date;
}
/**
 * @brief Set the uint32 data import tariff activation date object
 * 
 * @param data 
 * @param len 
 * @return DLMS_ERROR_CODE 
 */
DLMS_ERROR_CODE SET_UINT32_DATA_IMPORT_TARIFF_ACTIVATION_DATE(unsigned char* data,int* len)
{

	uint8_t activation_month = 0;
	uint16_t activation_year = 0;
	activation_month = (*(uint32_t*)data)/10000;
	activation_year = (*(uint32_t*)data)%10000;
    return billing_setTariffActivationDate(activation_month,activation_year);
}
/**
 * @brief Get the uint32 data export tariff activation date object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_UINT32_DATA_EXPORT_TARIFF_ACTIVATION_DATE(int* len)
{
    *len =4;
	static uint32_t id= 63;
	return &id;
}
/**
 * @brief Set the uint32 data export tariff activation date object
 * 
 * @param data 
 * @param len 
 * @return DLMS_ERROR_CODE 
 */
DLMS_ERROR_CODE SET_UINT32_DATA_EXPORT_TARIFF_ACTIVATION_DATE(unsigned char* data,int* len)
{
    return DLMS_ERROR_CODE_OK;
}

/**
 * @brief Get the int64 data cumulative recharge amount object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_INT64_DATA_CUMULATIVE_RECHARGE_AMOUNT(int* len)
{
    *len =8;
	static int64_t ChargeAmount = 0;
	billing_GetTotalChargeValue(&ChargeAmount);
	return &ChargeAmount;
}
/**
 * @brief Get the int32 data last recharge amount object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_INT32_DATA_LAST_RECHARGE_AMOUNT(int* len)
{
	*len = 4;
	static int32_t ChargeAmount = 0;
	billing_GetLastChargeValue(&ChargeAmount);
	return &ChargeAmount;
}
/**
 * @brief Get the uint32 data tid object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_UINT32_DATA_TID(int* len)
{
	*len = 4;
	static int32_t ChargeSequence = 0;
	billing_GetLastChargeSequenceNumber(&ChargeSequence);
	return (&ChargeSequence);
}

/**
 * @brief Get the int64 data remaining credit object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_INT64_DATA_Remaining_credit(int* len)
{
	*len = 8;
	extern billing_billingCycle billing_currentBillingCycle;
	static int64_t remaining_credit = 0;
	remaining_credit = ROUND_2_DEC(billing_currentBillingCycle.remainingCreditBalance);
	return &remaining_credit;
}
/**
 * @brief Get the uint64 data charge token gateway object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_UINT64_DATA_Charge_Token_Gateway(int* len)
{
	*len = 8;
	static uint64_t ChargeSequence = 0;
	return &ChargeSequence;
}
/**
 * @brief Set the uint64 data charge token gateway object
 * 
 * @param data 
 * @param len 
 * @return DLMS_ERROR_CODE 
 */
DLMS_ERROR_CODE SET_UINT64_DATA_Charge_Token_Gateway(unsigned char* data,int* len)
{
	*len = 8;
	/* first 4 bytes for chargeNumber and second 4 bytes for chargeAmount */
	uint32_t chargeSequence = *(uint32_t*)(data + 4);
	int32_t chargeAmount = *(int32_t*)(data);
	DLMS_ERROR_CODE return_v = billing_chargeCredit(chargeSequence, chargeAmount);
	return return_v;
}
/**
 * @brief Get the uint8 data tariff mode object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_UINT8_DATA_Tariff_Mode(int* len)
{
	*len = 1;
	extern billing_stepTariffPlan billing_currentStepTariffPlan; 
	return &(billing_currentStepTariffPlan.stepFeeDeductionRule);
}
/**
 * @brief Set the uint8 data tariff mode object
 * 
 * @param data 
 * @param len 
 * @return DLMS_ERROR_CODE 
 */
DLMS_ERROR_CODE SET_UINT8_DATA_Tariff_Mode(unsigned char* data,int* len)
{
	*len = 1;
	return billing_changeBillingPaymentMode(*data);
}
/**
 * @brief Get the uint8 data tariff current step object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_UINT8_DATA_Tariff_current_step(int* len)
{
	*len =1;
	extern billing_billingCycle billing_currentBillingCycle;
	return &billing_currentBillingCycle.currentStep;
}

uint8_t eventreset = 0;
/**
 * @brief Get the uint8 data tamper reset object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_UINT8_DATA_Tamper_reset(int* len)
{
	*len = 4;
	return &eventreset;
}
/**
 * @brief Set the uint8 data tamper reset object
 * 
 * @param data 
 * @param len 
 * @return DLMS_ERROR_CODE 
 */
DLMS_ERROR_CODE SET_UINT8_DATA_Tamper_reset(unsigned char* data,int* len)
{
	*len = 4;
	eventreset = (*(uint8_t*)data);
	DLMS_ERROR_CODE return_v;
	if(eventreset < EVENTS_ARRAY_NUM)
	{
		return_v = relay_RemoteEventReset(eventreset);
	}
	else if(eventreset == 32)
	{
		for (int i = 0;i < EVENTS_ARRAY_NUM; i++)
		{
			return_v = relay_RemoteEventReset(i);
			if(return_v != DLMS_ERROR_CODE_OK) break;
		}
	}
	else
	{
		return DLMS_ERROR_CODE_INVALID_COMMAND;
	}

	return return_v;
}

/**
 * @brief Get the string data apn object
 * 
 * @param len 
 * @return unsigned* 
 */

unsigned char* GET_STRING_DATA_APN(int* len)
{
	*len=strlen(apn_name);
	return &apn_name[0];
}
/**
 * @brief Set the string data apn object
 * 
 * @param data 
 * @param len 
 * @return DLMS_ERROR_CODE 
 */
DLMS_ERROR_CODE SET_STRING_DATA_APN(uint8_t * data,uint8_t * len)
{
	strcpy(apn_name,data);
	apn_name[*len] = 0;
	 
	eeprom_instantaneous_save(EEPROM_ARRAY_APN);
	module_reboot();
	return DLMS_ERROR_CODE_OK;
}
/**
 * @brief Get the string data apn username object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_STRING_DATA_APN_USERNAME(int* len)
{
	static char* username = "Username";
	*len = strlen(username);
	return username;	
}
/**
 * @brief Get the string data apn password object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_STRING_DATA_APN_PASSWORD(int* len)
{
	static char* password = "Password";
	*len = strlen(password);
	return password;	
}
/**
 * @brief Get the uint8 data weekend object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_UINT8_DATA_weekend(uint8_t* len)
{
	*len = 1;
	static uint8_t id = 0;
	extern billing_WeekDays billing_weekends;

	id = billing_weekends;
	return &id;
}

/**
 * @brief Set the uint8 data weekend object
 * 
 * @param data 
 * @param len 
 * @return DLMS_ERROR_CODE 
 */
DLMS_ERROR_CODE SET_UINT8_DATA_weekend(unsigned char* data , int* len)
{
	billing_WeekDays newWeekends = (billing_WeekDays)(*data);
	if (newWeekends <= ALL_DAYS)
	{
		billing_setWeeklyOffDays(newWeekends);

		return DLMS_ERROR_CODE_OK;
	}
	else
	{
		return DLMS_ERROR_CODE_INVALID_COMMAND;
	}
}

static uint16_t last_added_holiday;
/**
 * @brief Get the uint16 data add holiday object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_UINT16_DATA_add_holiday(uint8_t* len)
{
	*len = 2;
	return &last_added_holiday;
}
/**
 * @brief Set the uint16 data add holiday object
 * 
 * @param data 
 * @param len 
 * @return DLMS_ERROR_CODE 
 */
DLMS_ERROR_CODE SET_UINT16_DATA_add_holiday(unsigned char* data , int* len)
{
	last_added_holiday = (*(uint16_t*)data);
	if((last_added_holiday%100 <1)||(last_added_holiday%100 >31) /*    check valid day */
	||(last_added_holiday/100 <1)||(last_added_holiday/100 >12) ) /*   check valid month */
	{
		return DLMS_ERROR_CODE_INVALID_DATE_TIME;
	}

	return billing_AddHoliday(last_added_holiday%100,last_added_holiday/100);
}

static uint16_t last_removed_holiday;
/**
 * @brief Get the uint16 data remove holiday object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_UINT16_DATA_remove_holiday(uint8_t* len)
{
	*len = 2;
	return &last_removed_holiday;
}
/**
 * @brief Set the uint16 data remove holiday object
 * 
 * @param data 
 * @param len 
 * @return DLMS_ERROR_CODE 
 */
DLMS_ERROR_CODE SET_UINT16_DATA_remove_holiday(unsigned char* data , int* len)
{
	last_removed_holiday = (*(uint16_t*)data);
	if((last_removed_holiday%100 <1)||(last_removed_holiday%100 >31)   /*   check valid day */
		||(last_removed_holiday/100 <1)||(last_removed_holiday%100 >12) )/*    check valid month  */
	{
		return DLMS_ERROR_CODE_INVALID_DATE_TIME;
	}

	return billing_removeHoliday(last_added_holiday%100,last_added_holiday/100);
}
/**
 * @brief Get the arr data all holidays object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_ARR_DATA_all_holidays(uint8_t* len)
{
	extern billing_calendar billing_holidays;
	static gxByteBuffer holidays_of_the_year = {0};
	cosem_setArray(&holidays_of_the_year,12);
	for (int i = 1; i <= 12;i++)
	{
		cosem_setStructure(&holidays_of_the_year,2);
		cosem_setUInt8(&holidays_of_the_year,i);   /*month no*/
		cosem_setUInt32(&holidays_of_the_year,billing_holidays.months[i-1]);
	}
	*len = holidays_of_the_year.size;
	return &holidays_of_the_year;
}

/**
 * @brief Set the arr data all holidays object
 * 
 * @param data 
 * @param len 
 * @return DLMS_ERROR_CODE 
 */
DLMS_ERROR_CODE SET_ARR_DATA_all_holidays(unsigned char* data , int* len)
{
	dlmsVARIANT** arrayofstructs = (dlmsVARIANT**)data;
	extern billing_calendar billing_holidays;
	extern billing_calendar billing_offDays;
	uint8_t local_temp_for_month;
	dlmsVARIANT** ptr_to_struct_data;
	
	/*check if the months are sent in the right order*/
	for(uint8_t local_month_index = 0; local_month_index < *len; local_month_index++)
	{
		/* ptr_to_struct_data points to array of pointers to structure data */
		ptr_to_struct_data = (arrayofstructs[local_month_index]->Arr->data);
		var_getUInt8(ptr_to_struct_data[0],&local_temp_for_month);
		/* if months aren't send in the right order it will return error*/
		if (local_temp_for_month != local_month_index+1)
		{
			return DLMS_ERROR_CODE_UNACCEPTABLE_FRAME;
		}
	}
	billing_resetHolidays(); /*   to remove previous holidays from off days  */
	for(uint8_t local_month_index = 0; local_month_index < *len; local_month_index++)
	{
		ptr_to_struct_data = (arrayofstructs[local_month_index]->Arr->data);
		var_getUInt32(ptr_to_struct_data[1],&billing_holidays.months[local_month_index]);
		billing_offDays.months[local_month_index]|= billing_holidays.months[local_month_index];
	} 
	eeprom_instantaneous_save(EEPROM_STRUCT_BILLING_HOLIDAYS);
	return DLMS_ERROR_CODE_OK;
}

static uint16_t start_friendly_time;

/**
 * @brief Get the uint16 data start friendly time object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_UINT16_DATA_start_friendly_time(uint8_t* len)
{
	*len = 2;
	extern billing_friendlytime_t billing_start_friendlytime;
	start_friendly_time = (uint16_t)(billing_start_friendlytime.hour *100) + billing_start_friendlytime.minute;
	return &start_friendly_time;
}

/**
 * @brief Set the uint16 data start friendly time object
 * 
 * @param data 
 * @param len 
 * @return DLMS_ERROR_CODE 
 */
DLMS_ERROR_CODE SET_UINT16_DATA_start_friendly_time(unsigned char* data , int* len)
{
	start_friendly_time = (*(uint16_t*)data);
	if(    (start_friendly_time%100 >59)  /*   check valid minutes */
		|| (start_friendly_time/100 >23) ) /*   check valid hours */
	{
		return DLMS_ERROR_CODE_INVALID_DATE_TIME;
	}
	billing_setStartFriendlyTime(start_friendly_time/100,last_added_holiday%100);

	return DLMS_ERROR_CODE_OK;
}

static uint16_t end_friendly_time;
/**
 * @brief Get the uint16 data end friendly time object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_UINT16_DATA_end_friendly_time(uint8_t* len)
{
	*len = 2;
	extern billing_friendlytime_t billing_end_friendlytime;

	end_friendly_time = (uint16_t)(billing_end_friendlytime.hour *100) + billing_end_friendlytime.minute;
	return &end_friendly_time;
}
/**
 * @brief Set the uint16 data end friendly time object
 * 
 * @param data 
 * @param len 
 * @return DLMS_ERROR_CODE 
 */
DLMS_ERROR_CODE SET_UINT16_DATA_end_friendly_time(unsigned char* data , int* len)
{
	end_friendly_time = (*(uint16_t*)data);
	if(    (end_friendly_time%100 >59)   /*   check valid minutes */
		|| (end_friendly_time/100 >23) ) /*   check valid hours */
	{
		return DLMS_ERROR_CODE_INVALID_DATE_TIME;
	}
	billing_setEndFriendlyTime(end_friendly_time/100,end_friendly_time%100);

	return DLMS_ERROR_CODE_OK;
}


/**
 * @brief Get the struct data test mode object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_STRUCT_DATA_TEST_MODE(int *len)
{
	static gxByteBuffer testStructstatus ;

	int return_v =  cosem_setStructure(&testStructstatus,6);

	cosem_setUInt8(&testStructstatus,  test_mode_status.Relay);
	cosem_setUInt8(&testStructstatus,  test_mode_status.eeprom);
	cosem_setUInt8(&testStructstatus,  test_mode_status.extFlash);
	cosem_setUInt8(&testStructstatus,  test_mode_status.secureElement);
	cosem_setUInt8(&testStructstatus,  test_mode_status.Backlight);
	cosem_setUInt8(&testStructstatus,  test_mode_status.screen_icons);
	
	*len = testStructstatus.size;
	return &testStructstatus;
	
}


/**
 * @brief Set the struct data test mode object
 * 
 * @param data 
 * @param len 
 * @return DLMS_ERROR_CODE 
 */
DLMS_ERROR_CODE SET_STRUCT_DATA_TEST_MODE(unsigned char* data,int* len)
{
	/** TODO:: handle test mode **/
	#if 0
	Relay_close();/*    close	Relay First */
	
	backlight_on();/*    backLight on */
	clear_Lcd_momery(); /*   clear lcd */
	lcd_write_ln(AllData[DATA_TEST_MODE].base.logicalName);    write test mode LN on LCD
	test_mode_on(); /*   write "test on" on LCD */
	lcd_write(10,1); /*   write test number */
	LCD_Write_Value_digit();
	
	/* test Relay test 1*/
	delay_ms(2000);
	Relay_open();
	delay_ms(2000);
	Relay_close();
	delay_ms(2000);
	
	test_mode_status.Relay = 1; /*   relay test pass */
	
	/*test external Flash test 2*/
	lcd_clear_digit(10);/*    clear digit 10 */
	lcd_write(10,2); /*   write test number */
	LCD_Write_Value_digit();
	uint8_t flashData[100];
	ext_flash_ReadBytes(0x1000, 100, flashData); /*   read external flash from addr 0x1000 */
	delay_ms(2000);
	if(flashData != 0){
		test_mode_status.extFlash = 1; /*   external flash test pass */
	}
	else{
		test_mode_status.extFlash = 0;/*    external flash test didn't pass */
	}
	
	/*test secure element test 3*/
	lcd_clear_digit(10); /*   clear digit 10 */
	lcd_write(10,3); /*   write test number */
	LCD_Write_Value_digit();
	/*test secure element*/
	if(getInfo()==1){
		test_mode_status.secureElement = 1;
	}
	else{
		test_mode_status.secureElement = 0;
	}
	delay_ms(2000);
	
	
	/*  todo Marwan: valid eeprom address to be read */
	#if 0
	/*test eeprom test 4*/
	lcd_clear_digit(10);
	lcd_write(10,4);
	LCD_Write_Value_digit();
	uint8_t buf[6]={0};
	uint16_t  e2addr = ( E2Pre1Event_PowerCut_PgAddr << 8 ) + E2Pre1Event_PowerCut_StartTmAdr;
	BSP_E2promBus_Init();
	EEPROMreader(e2addr,&buf[0],5);
	if((buf != 0)&&(buf[0] != 0xFF)){
		
		test_mode_status.eeprom = 1;
	}
	else{
		test_mode_status.eeprom = 0;
	}
	delay_ms(2000);
	
	#endif
	
	/* test screen icons*/
	lcd_clear_digit(10);
	lcd_write(10,5);
	LCD_Write_Value_digit();
	delay_ms(2000);
	test_screen_icons();
	test_mode_status.screen_icons=1;
	test_mode_status.Backlight=1;
	
	/* test is end*/
	clear_Lcd_momery();
	test_mode_off();
	LCD_Write_Value_digit();
	delay_ms(1000);
	backlight_off();
	#endif
}



uint8_t billing_index;
/**
 * @brief Get the uint16 data billing history index object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_UINT16_DATA_BILLING_HISTORY_INDEX(int* len)
{
	len = 1;
	return &(billing_index);
}
/**
 * @brief Set the uint16 data billing history index object
 * 
 * @param data 
 * @param len 
 * @return DLMS_ERROR_CODE 
 */
DLMS_ERROR_CODE SET_UINT16_DATA_BILLING_HISTORY_INDEX(unsigned char* data , int* len){
	
	if(*(uint8_t*)data > 25)
	{
		return DLMS_ERROR_CODE_DATA_BLOCK_UNAVAILABLE;	
	}
	else
	{
		billing_index = *(uint8_t*)data;
	}
	
	return DLMS_ERROR_CODE_OK;
}
/**
 * @brief Get the stuct data billing history object
 * 
 * @param len 
 * @return unsigned* 
 */
unsigned char* GET_STUCT_DATA_BILLING_HISTORY(int* len)
{
	static gxByteBuffer billingHistoryBuffer;
	extern billing_billingCycle billing_billingCyclesHistory[NO_OF_BILLING_CYCLES];
	extern billing_billingCycle billing_currentBillingCycle;
	cosem_setStructure(&billingHistoryBuffer,13);
	
	if (billing_index == 0)
	{
		cosem_setUInt8(&billingHistoryBuffer, billing_currentBillingCycle.month);
		cosem_setUInt16(&billingHistoryBuffer,billing_currentBillingCycle.year);
		cosem_setUInt32(&billingHistoryBuffer,ROUND_2_DEC(billing_currentBillingCycle.openingCreditBalance));
		cosem_setUInt8(&billingHistoryBuffer, billing_currentBillingCycle.totalChargesCount);
		cosem_setUInt32(&billingHistoryBuffer,ROUND_2_DEC(billing_currentBillingCycle.totalChargesValue));
		cosem_setUInt32(&billingHistoryBuffer,ROUND_2_DEC(billing_currentBillingCycle.consumptionValue));
		cosem_setUInt8(&billingHistoryBuffer, billing_currentBillingCycle.currentStep);
		cosem_setUInt32(&billingHistoryBuffer,ROUND_2_DEC(billing_currentBillingCycle.totalStepFees));
		cosem_setUInt32(&billingHistoryBuffer,ROUND_2_DEC(billing_currentBillingCycle.remainingCreditBalance));
		cosem_setUInt16(&billingHistoryBuffer,billing_currentBillingCycle.Forward_KWH);
		cosem_setUInt8(&billingHistoryBuffer, billing_currentBillingCycle.Forward_fraction);
		cosem_setUInt16(&billingHistoryBuffer,billing_currentBillingCycle.Backward_KWH);
		cosem_setUInt8(&billingHistoryBuffer, billing_currentBillingCycle.Backward_fraction);
		
	}
	else
	{
		cosem_setUInt8(&billingHistoryBuffer, billing_billingCyclesHistory[billing_index].month);		
		cosem_setUInt16(&billingHistoryBuffer,billing_billingCyclesHistory[billing_index].year);
		cosem_setUInt32(&billingHistoryBuffer,ROUND_2_DEC(billing_billingCyclesHistory[billing_index].openingCreditBalance));	
		cosem_setUInt8(&billingHistoryBuffer, billing_billingCyclesHistory[billing_index].totalChargesCount);
		cosem_setUInt32(&billingHistoryBuffer,ROUND_2_DEC(billing_billingCyclesHistory[billing_index].totalChargesValue));
		cosem_setUInt32(&billingHistoryBuffer,ROUND_2_DEC(billing_billingCyclesHistory[billing_index].consumptionValue));
		cosem_setUInt8(&billingHistoryBuffer, billing_billingCyclesHistory[billing_index].currentStep);		
		cosem_setUInt32(&billingHistoryBuffer,ROUND_2_DEC(billing_billingCyclesHistory[billing_index].totalStepFees));
		cosem_setUInt32(&billingHistoryBuffer,ROUND_2_DEC(billing_billingCyclesHistory[billing_index].remainingCreditBalance));	
		cosem_setUInt16(&billingHistoryBuffer,billing_billingCyclesHistory[billing_index].Forward_KWH);	
		cosem_setUInt8(&billingHistoryBuffer, billing_billingCyclesHistory[billing_index].Forward_fraction);		
		cosem_setUInt16(&billingHistoryBuffer,billing_billingCyclesHistory[billing_index].Backward_KWH);
		cosem_setUInt8(&billingHistoryBuffer, billing_billingCyclesHistory[billing_index].Backward_fraction);			
	}
	*len = billingHistoryBuffer.size;
	return &billingHistoryBuffer;
}


/**
 * @brief 
 * 
 * @param data 
 * @param len 
 * @return DLMS_ERROR_CODE 
 */
DLMS_ERROR_CODE SET_Meter_GLOBAL_RESET(unsigned char* data,int* len){

	return DLMS_ERROR_CODE_OK;
}

