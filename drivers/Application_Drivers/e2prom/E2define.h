
#ifndef		_E2define_h
#define		_E2define_h


#ifdef __cplusplus
extern "C" {
#endif




/************************************************************************/
/*						lookup table start	Page 64						*/
/*						lookup table end	Page 74						*/
/************************************************************************/

#define     EEPROM_FIRST_START_FLAG					0x4000
#define     EEPROM_LOOKUP_TABLE_LAST_INDEX			EEPROM_FIRST_START_FLAG + 2
#define     EEPROM_LOOKUP_TABLE_START_ADDRESS		EEPROM_LOOKUP_TABLE_LAST_INDEX +2  /**start address of the lookup table **/



/************************************************************************/
/*							data start	Page 74							*/
/************************************************************************/

#define     SAVED_DATA_START_ADDRESS        0x4A00  /** start address of the data **/
#define     EEPROM_LAST_ADDRESS				0x8000  /** last address of eeprom **/



/*============================================================================*/




/*Write the three images*/
#define BILLING_HISTORY_BASE_ADDRESS        (uint16_t)0x3140
#define BILLING_HISTORY_MONTH_STEP          (uint16_t)0x230
#define BILLING_HISTORY_IMAGE_STEP          (uint16_t)0x1A40
#define BILLING_HISTORY_IMAGE_1_ADDRESS     BILLING_HISTORY_BASE_ADDRESS
#define BILLING_HISTORY_IMAGE_2_ADDRESS     BILLING_HISTORY_BASE_ADDRESS + (BILLING_HISTORY_IMAGE_STEP * 1)
#define BILLING_HISTORY_IMAGE_3_ADDRESS     BILLING_HISTORY_BASE_ADDRESS + (BILLING_HISTORY_IMAGE_STEP * 2)




/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif


#endif		/*//E2define.h*/

