/**
 * @file billing.h
 * @author Omar Radi
 * @brief Billing Logic Header File
 * @version 0.1
 * @date 2020-07-14 11:22:20 PM
 * 
 * @copyright Copyright (c) 2020
 * 
 */



#ifndef BILLING_H_
#define BILLING_H_

#include "stdint.h"
#include "dlms.h"

/* Note any change in no of billing cycles or no of tariff steps will affect eeprom saving*/
#define NO_OF_BILLING_CYCLES 24 /*  Max 24 */

#define NO_OF_TARIFF_STEPS 10	/*  any change will affect eeprom */


typedef struct __attribute__((packed))
{	
	double consumptionValue;			/*  LE */
	double openingCreditBalance;		/*  LE */
	double totalChargesValue;			/*  LE */
	double remainingCreditBalance;		/*  LE */
	float totalStepFees;				/*  LE */
	uint16_t Forward_KWH;				/*  120amps*240volts*24hours*31days 21,427KWH ( x3 for 3phases) */
	uint16_t Backward_KWH;				/*  120amps*240volts*24hours*31days 21,427KWH ( x3 for 3phases)  not handled yet */
	uint16_t year;						/*  store the date when the billing cycle started */
	uint8_t month;
	uint8_t totalChargesCount;			/*  Count of charges per billing cycle */
	uint8_t currentStep;				/*  Keep it here to handle the case of switching back and forth the date to the same cycle, snapshot last status */
	uint8_t Forward_fraction;			/*  Calculate the fraction consumption at the end of the billing cycle */
	uint8_t Backward_fraction;			/*  Calculate the fraction consumption at the end of the billing cycle  not handled yet */
} billing_billingCycle;

/**
 * @brief Meter lifetime registers that should never be formatted, even if the meter is re-commissioned for a new customer
 * 
 */
typedef struct __attribute__((packed))
{
	double totalChargesValue;	/* LE */
	double totalDebtValue;		/* LE */
	double LastChargeValue;		/* LE */
	double Forward_KWH;			/* 120amps * 240volts * 24hours * 31days * 12month * 20year = 5,142,480KWH ( x3 for 3phases) */
	double Backward_KWH;		/* 120amps * 240volts * 24hours * 31days * 12month * 20year = 5,142,480KWH ( x3 for 3phases)  not handled yet */
	uint32_t totalChargesCount;
	uint32_t LastChargeSequence;
	uint32_t ID;
	uint8_t billingCyclesCount;
} billing_totalsRegisters;


typedef struct   __attribute__((packed))
{
	int16_t stepPrice; /* LE *100 */
	int16_t stepFee;   /* LE *100 */
	uint16_t toKWH;  /* KWH not WH	 */
	uint8_t stepNo;  /* For LCD Display */
	uint8_t recalculationFlag;
} billing_tariffStep;

typedef struct __attribute__((packed))
{
	int16_t zeroConsumptionFee; 
	uint8_t stepFeeDeductionRule; /* 1 -> per step 2 -> per month */
	uint8_t activationMonth; /* base date (days[0] LSB)  */
	uint16_t activationYear; 
	uint16_t activationCode; /*  used for tariff activation */
	billing_tariffStep steps[NO_OF_TARIFF_STEPS];

} billing_stepTariffPlan;

typedef enum
{
	NO_DAY	  = 0,
	SATURDAY  = 1,
	SUNDAY    = 2,
	MONDAY    = 4,
	TUESDAY   = 8,
	WEDNSDAY  = 16,
	THURSDAY  = 32,
	FRIDAY    = 64,
	ALL_DAYS  = 127,
}billing_WeekDays;


typedef struct __attribute__((packed))
{
	uint32_t months[12];
	uint16_t base_year;
	uint8_t base_month; /* first day of the month , saturday = 0 -> friday = 6 */
	uint8_t week;
} billing_calendar;

typedef struct __attribute__((packed))
{
	uint8_t hour;
	uint8_t minute;		
} billing_friendlytime_t;


typedef enum
{
	TARIFF_NOT_SET	= 0,
	PREPAID  = 1,
	POSTPAID = 2,
}billing_payment_mode;


/* =========================================================== billingcycles APIs =================================================================== */

void billing_initBillingCycle(void);
void billing_closeCurrentBillingCycle(void);
int billing_isNewBillingCycle(void);

/*======================================================== Registers APIs ===================================================================*/
/**
 * @brief reset for new customer
 * @TODO saving data to eeprom
 */
void billing_ResetForNewCustomer(uint32_t New_ID);
/* =========================================================== credit calculations APIs ============================================================= */

void billing_GetTotalChargeValue(int64_t* returnChargeValue);
void billing_GetLastChargeValue(int32_t* returnChargeValue);
void billing_GetLastChargeSequenceNumber(uint32_t* returnChargeSequence);
DLMS_ERROR_CODE billing_chargeCredit(uint32_t chargeNumber,int32_t creditAmount);
DLMS_ERROR_CODE billing_changeBillingPaymentMode(billing_payment_mode newMode);

/*=========================================================== tariff config APIs ====================================================================*/

/******************************************************************************
	Description     ::		calculate consumption fee from new consumption value
	Function        ::		billing_processTariff
	Input           ::		Forward_consumption: import energy, Backward_consumption: export
	Output          ::		none
	Call            ::		billing_processTariff_PerKWH, billing_RelayControl
 ******************************************************************************/
void billing_processTariff(uint32_t Forward_consumption,uint32_t Backward_consumption);

/******************************************************************************
	Description     ::		set sandby tariff activation date
	Function        ::		billing_setTariffActivationDate
	Input           ::		month, year
	Output          ::		DLMS_ERROR_CODE error check
	Call            ::		rtc_get_date
 ******************************************************************************/
DLMS_ERROR_CODE billing_setTariffActivationDate(uint8_t month,uint16_t year);

/******************************************************************************
	Description     ::		check if the time of activate come to activate new tariff
	Function        ::		billing_isTariffActivationTime
	Input           ::		none
	Output          ::		yes = 1, no =0
	Call            ::		rtc_get_date
 ******************************************************************************/
int billing_isTariffActivationTime(void);

/******************************************************************************
	Description     ::		copy future tariff to active tariff
	Function        ::		billing_ActivateNewTariff
	Input           ::		none
	Output          ::		none
	Call            ::		rtc_get_date, eeprom_instantaneous_save
 ******************************************************************************/
void billing_ActivateNewTariff(void);

/* =========================================================== off days APIs ========================================================================= */

/******************************************************************************
 Description     ::		set weekends
 Function        ::		billing_setWeeklyOffDays
 Input           ::		new_weeklyOffDays : use enum WeekDays ,ex: SATURDAY + FRIDAY
 Output          ::		none
 Call            ::		none
 ******************************************************************************/
void billing_setWeeklyOffDays(billing_WeekDays new_weeklyOffDays);

/******************************************************************************
 Description     ::		set OffDays if year, holidays, weekoffdays changed
 Function        ::		billing_updateOffDays
 Input           ::		none
 Output          ::		none
 Call            ::		day_of_week, rtc_get_date
 Effect          ::		update offDays object
 ******************************************************************************/
void billing_updateOffDays(void);

/******************************************************************************
 Description     ::		add holiday to holidays object
 Function        ::		billing_AddHoliday
 Input           ::		day, month
 Output          ::		DLMS_ERROR_CODE_OK
 Call            ::		none
 ******************************************************************************/
DLMS_ERROR_CODE billing_AddHoliday(uint8_t day,uint8_t month);

/******************************************************************************
 Description     ::		remove holiday from holidays object
 Function        ::		billing_removeHoliday
 Input           ::		day, month
 Output          ::		DLMS_ERROR_CODE_OK
 Call            ::		none
 ******************************************************************************/
DLMS_ERROR_CODE billing_removeHoliday(uint8_t day,uint8_t month);

/******************************************************************************
 Description     ::		reset holidays object
 Function        ::		billing_resetHolidays
 Input           ::		none
 Output          ::		none
 Call            ::		rtc_get_date
 ******************************************************************************/
void billing_resetHolidays(void);

/******************************************************************************
 Description     ::		set start of friendly time range
 Function        ::		billing_setStartFriendlyTime
 Input           ::		start_hour, start_minutes, end_hours, end_minutes
 Output          ::		none
 Call            ::		none
 Note			  ::		time must be in 24 mode, if start time equals end time this exact minute will be the friendly time.
 ******************************************************************************/
void billing_setStartFriendlyTime(uint8_t start_hour,uint8_t start_minutes);

/******************************************************************************
 Description     ::		set end of friendly time range
 Function        ::		billing_setEndFriendlyTime
 Input           ::		start_hour, start_minutes, end_hours, end_minutes
 Output          ::		none
 Call            ::		none
 Note			  ::		time must be in 24 mode, if start time equals end time this exact minute will be the friendly time.
 ******************************************************************************/
void billing_setEndFriendlyTime(uint8_t end_hours,uint8_t end_minutes);

/******************************************************************************
 Description     ::		check if rtc is in friendly time
 Function        ::		billing_isFriendlyTime
 Input           ::		start_hour, start_minutes, end_hours, end_minutes
 Output          ::		none
 Call            ::		rtc_get_time, checkTimeInRange, checkTimeOutRange
 ******************************************************************************/
int billing_isFriendlyTime(void);

/******************************************************************************
 Description     ::		return if the day is off or not
 Function        ::		billing_isOffDay
 Input           ::		nont
 Output          ::		0: false, 1: true
 Call            ::		none
 ******************************************************************************/
int billing_isOffDay(void);

/******************************************************************************
	 not implemented yet
 ******************************************************************************/
int billing_isGraceHours(void);

/******************************************************************************
	 not implemented yet
 ******************************************************************************/
int billing_isGracePeriod(void);
		
/**=========================================================== relay control APIs ======================================================================*/

/*******************************************************************************************
 * @brief Connect and Disconnect Relay
 * Should be called every 1KWH of consumption,billing cycle closure,
 * charge, discharge, every minute (xx:xx:00) after isNewBillingCycle(void) check (new bc or setDateTime)
 *************************************************************/
void billing_RelayControl(void);



#endif /* BILLING_H_ */
