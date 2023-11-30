/**
 * @file billing.c
 * @author Omar Radi
 * @brief Billing Logic
 * @version 0.1
 * @date 2020-07-14 11:22:20 PM
 *
 * @copyright Copyright (c) 2020
 *
 * @contributor Youssef Mohamed
 * @date 2023-08-10
 */

#include "billing.h"
#include "math.h"
#include "RTC_Wrapper/rtc_wrapper.h"
#include "solenoid.h"
#include "EEPROM.h"

#define SET_BIT(var, bit) ((var) |= (1 << (bit)))
#define CLEAR_BIT(var, bit) ((var) &= ~(1 << (bit)))
#define Get_BIT(var, bit) (((var) >> (bit)) & 1)
#define WEEK_CYCLIC_SHIFT(week, shift) (((week) >> (shift)) | (((week) << (7 - (shift))) & 0x7F))

/* APIs to help in consumption calculation */
static void applyStepFee(void);
static void applyStepFeeAfterRecalculation(void);
static void applyStepFeeOnClosing(void);
static void applyZeroConsumptionFee(void);
static void settleKwhFraction(void);
static void updateRemainingCredit(void);
static void billing_processTariff_PerKWH(void);

/* used in off days calculations */
uint8_t day_of_week(uint8_t m, uint8_t d, uint16_t y);

billing_totalsRegisters billing_lifetimeTotalsRegisters;
billing_totalsRegisters billing_customerTotalsRegisters; /* must be reinitialized with every new customer */

billing_stepTariffPlan billing_currentStepTariffPlan; /* The currently configured step tariff plan */
billing_stepTariffPlan billing_futureStepTariffPlan;  /* the future step tariff plan */

billing_billingCycle billing_currentBillingCycle; /* The currently active billing cycle */
billing_billingCycle billing_billingCyclesHistory[NO_OF_BILLING_CYCLES];

billing_calendar billing_offDays;
billing_calendar billing_holidays;
billing_WeekDays billing_weekends;

billing_friendlytime_t billing_start_friendlytime;
billing_friendlytime_t billing_end_friendlytime;

uint8_t billing_friendlytime_reversed;

static uint8_t force_update = 0;
/*=========================================================== billingcycles APIs =================================================================== */

/**
 * @brief Open and initialize a new billing cycle
 * Should be invoked on 00:00 of a new billing cycle, or a set datetime command that changes the date to a new billing cycle
 * @return none
 */
void billing_initBillingCycle(void)
{
	/* Initialize the opening credit balance with the previous cycle ending balance */
	uint8_t rtc_month = 0;
	uint16_t rtc_year = 0;

	rtc_get_date(&rtc_year, &rtc_month, NULL, NULL);

	billing_currentBillingCycle.month = rtc_month;
	billing_currentBillingCycle.year = rtc_year;

	billing_currentBillingCycle.openingCreditBalance += billing_currentBillingCycle.totalChargesValue - billing_currentBillingCycle.consumptionValue;
	billing_currentBillingCycle.remainingCreditBalance = billing_currentBillingCycle.openingCreditBalance;

	billing_currentBillingCycle.currentStep = 0;
	billing_currentBillingCycle.consumptionValue = 0;
	billing_currentBillingCycle.Forward_fraction = 0;
	billing_currentBillingCycle.totalChargesValue = 0;
	billing_currentBillingCycle.totalChargesCount = 0;
	billing_currentBillingCycle.totalStepFees = 0;
	billing_currentBillingCycle.Forward_KWH = 0;

	 
	eeprom_instantaneous_save(EEPROM_STRUCT_BILLING_CURRENT_BILLING_CYCLE);
	remove_event(EVENT_NAME_BILLING_RESET);
	set_event(EVENT_NAME_BILLING_RESET);
}

/**
 * @brief Close billing cycle
 *
 * @return none
 */
void billing_closeCurrentBillingCycle(void)
{

	/* Calculate the remaining KWH fraction value */
	settleKwhFraction();

	/* Apply last step fee while closing the billing cycle if payment_mode == postpaid */
	applyStepFeeOnClosing();

	/* Handle Zero Consumption */
	applyZeroConsumptionFee();

	/* Push and save billing_currentBillingCycle struct into billing_billingCyclesHistory */
	if (billing_currentBillingCycle.Forward_KWH > 0)
	{
		billing_lifetimeTotalsRegisters.billingCyclesCount++;
		billing_customerTotalsRegisters.billingCyclesCount++;
		for (int i = 0; i < NO_OF_BILLING_CYCLES - 1; i++) /* last billing cycle is cleared and the new one is saved in index zero */
		{
			billing_billingCyclesHistory[i + 1] = billing_billingCyclesHistory[i];
		}
		billing_billingCyclesHistory[0] = billing_currentBillingCycle;
	}

	billing_initBillingCycle(); /* clear current BillingCycle */
	 

	eeprom_instantaneous_save(EEPROM_STRUCT_BILLING_LIFE_TIME_REGISTERS);
	eeprom_instantaneous_save(EEPROM_STRUCT_BILLING_CUSTOMER_REGISTERS);
	eeprom_instantaneous_save(EEPROM_ARRAY_BILLING_BILLING_CYCLES_HISTORY);
}

/**
 * @brief Checks if the RTC has switched to a new billing cycle
 * either 00:00:00 of a new cycle, or setDateTime has been called
 *
 * @return int 0->False, 1->True
 */
int billing_isNewBillingCycle(void)
{
	/* TODO:: save previous_month and previous_year in eeprom */
	static uint8_t previous_month = 0;
	static uint16_t previous_year = 0;
	uint8_t month = 0;
	uint16_t year = 0;

	rtc_get_date(&year, &month, NULL, NULL);

	if (previous_year == year && previous_month == month)
	{
		return (0);
	}
	else if (previous_year < year || previous_month < month)
	{
		previous_year = year;
		previous_month = month;
		return (1);
	}
	return (0);
}

/*=========================================================== Registers APIs =================================================================== */

/**
 * @brief reset for new customer
 *
 */
void billing_ResetForNewCustomer(uint32_t New_ID)
{

	if (billing_customerTotalsRegisters.ID != New_ID)
	{
		memset(&billing_customerTotalsRegisters, 0, sizeof(billing_customerTotalsRegisters));
		billing_customerTotalsRegisters.ID = New_ID;
		billing_customerTotalsRegisters.billingCyclesCount = billing_lifetimeTotalsRegisters.billingCyclesCount;
		billing_initBillingCycle();
		/* save billing_customerTotalsRegisters */
		eeprom_instantaneous_save(EEPROM_STRUCT_BILLING_CUSTOMER_REGISTERS);
		eeprom_instantaneous_save(EEPROM_STRUCT_BILLING_CURRENT_BILLING_CYCLE);
	}
}

/*=========================================================== credit calculations APIs ============================================================= */

static void applyStepFee(void)
{
	billing_currentBillingCycle.totalStepFees += billing_currentStepTariffPlan.steps[billing_currentBillingCycle.currentStep].stepFee / 100.0;

	/* Apply step fee based on a flag (either on 1->step or 2->end of billing cycle) */
	if (billing_currentStepTariffPlan.stepFeeDeductionRule == PREPAID)
	{
		billing_currentBillingCycle.consumptionValue += billing_currentStepTariffPlan.steps[billing_currentBillingCycle.currentStep].stepFee / 100.0;
		eeprom_instantaneous_save(EEPROM_STRUCT_BILLING_CURRENT_BILLING_CYCLE);
	}
}

/**
 * @brief Re-add the previous fees that have been overwritten by the recalculation
 *
 */
static void applyStepFeeAfterRecalculation(void)
{

	if (billing_currentStepTariffPlan.stepFeeDeductionRule == PREPAID)
	{
		billing_currentBillingCycle.consumptionValue += billing_currentBillingCycle.totalStepFees;
		eeprom_instantaneous_save(EEPROM_STRUCT_BILLING_CURRENT_BILLING_CYCLE);
	}
}

static void applyStepFeeOnClosing(void)
{
	/* Apply step fee based on a flag (either on 1->step or 2->end of billing cycle) */
	if (billing_currentStepTariffPlan.stepFeeDeductionRule == POSTPAID)
	{
		billing_currentBillingCycle.consumptionValue += billing_currentBillingCycle.totalStepFees;
		eeprom_instantaneous_save(EEPROM_STRUCT_BILLING_CURRENT_BILLING_CYCLE);
	}
}

/**
 * @brief Apply fee when zero consumption (less than 1KWH) has been measured during the billing cycle
 *
 */
static void applyZeroConsumptionFee(void)
{
	if (billing_currentBillingCycle.Forward_KWH < 1)
	{
		billing_currentBillingCycle.consumptionValue += billing_currentStepTariffPlan.zeroConsumptionFee / 100.0;
		eeprom_instantaneous_save(EEPROM_STRUCT_BILLING_CURRENT_BILLING_CYCLE);
	}
}

/**
 * @brief Calculate KHW Fraction value and add it to the consumption value using current step price
 *
 */
static void settleKwhFraction(void)
{
	if (billing_currentBillingCycle.Forward_KWH > 0)
	{
		billing_currentBillingCycle.consumptionValue += billing_currentBillingCycle.Forward_fraction * billing_currentStepTariffPlan.steps[billing_currentBillingCycle.currentStep].stepPrice / 100.0;
		eeprom_instantaneous_save(EEPROM_STRUCT_BILLING_CURRENT_BILLING_CYCLE);
	}
}

/**
 * @brief get last charge amount
 * @param pointer to credit value
 */
void billing_GetTotalChargeValue(int64_t *returnChargeValue)
{
	*returnChargeValue = (int64_t)(billing_customerTotalsRegisters.totalChargesValue * 100);
}

/**
 * @brief get last charge amount
 * @param pointer to credit value
 */
void billing_GetLastChargeValue(int32_t *returnChargeValue)
{
	*returnChargeValue = (int32_t)(billing_customerTotalsRegisters.LastChargeValue * 100);
}

/**
 * @brief get last charge amount
 * @param pointer to credit value
 */
void billing_GetLastChargeSequenceNumber(uint32_t *returnChargeSequence)
{
	*returnChargeSequence = (uint32_t)(billing_customerTotalsRegisters.LastChargeSequence);
}

/**
 * @brief Charge the meter credit with the specified amount
 *
 * @param chargeNumber sequence of charges must be more than the last charge sequence number
			,creditAmount Amount to charge in piasters
 * @return int 1->Success
 */
DLMS_ERROR_CODE billing_chargeCredit(uint32_t chargeNumber, int32_t creditAmount)
{
	/* should be saved in eeprom */

	if (billing_lifetimeTotalsRegisters.LastChargeSequence >= chargeNumber)
	{

		return DLMS_ERROR_CODE_REJECTED;
	}
	billing_currentBillingCycle.totalChargesValue += ((creditAmount) / 100.0);
	billing_currentBillingCycle.totalChargesCount++;

	billing_lifetimeTotalsRegisters.totalChargesValue += ((creditAmount) / 100.0);
	billing_lifetimeTotalsRegisters.totalChargesCount++;

	billing_customerTotalsRegisters.totalChargesValue += ((creditAmount) / 100.0);
	billing_customerTotalsRegisters.totalChargesCount++;

	billing_lifetimeTotalsRegisters.LastChargeValue = (creditAmount / 100.0);
	billing_customerTotalsRegisters.LastChargeValue = (creditAmount / 100.0);

	/* update LastChargeNumber */
	billing_lifetimeTotalsRegisters.LastChargeSequence = chargeNumber;
	billing_customerTotalsRegisters.LastChargeSequence = chargeNumber;

	updateRemainingCredit();

	/* update data in eeprom */
	eeprom_instantaneous_save(EEPROM_STRUCT_BILLING_CUSTOMER_REGISTERS);
	eeprom_instantaneous_save(EEPROM_STRUCT_BILLING_LIFE_TIME_REGISTERS);
	eeprom_instantaneous_save(EEPROM_STRUCT_BILLING_CURRENT_BILLING_CYCLE);
	return DLMS_ERROR_CODE_OK;
}

/**
 * @brief Update the remaining credit register
 *
 * @return int
 */
static void updateRemainingCredit(void)
{
	billing_currentBillingCycle.remainingCreditBalance = billing_currentBillingCycle.openingCreditBalance + billing_currentBillingCycle.totalChargesValue - billing_currentBillingCycle.consumptionValue;
}

/******************************************************************************
 Description     ::		change payment mode instantaneuosly
 Function        ::		billing_changeBillingPaymentMode
 Input           ::		billing_payment_mode : PREPAID, POSTPAID
 Output          ::		DLMS_ERROR_CODE
 Call            ::		eeprom_instantaneous_save
 ******************************************************************************/
DLMS_ERROR_CODE billing_changeBillingPaymentMode(billing_payment_mode newMode)
{
	if (newMode == PREPAID || newMode == POSTPAID)
	{
		billing_currentStepTariffPlan.stepFeeDeductionRule = newMode;

		eeprom_instantaneous_save(EEPROM_STRUCT_BILLING_CURRENT_STEP_TARIFF_PLAN);
		return DLMS_ERROR_CODE_OK;
	}
	else
	{
		return DLMS_ERROR_CODE_REJECTED;
	}
}

/* =========================================================== taiff config APIs ================================================================= */

/**
 * @brief calculate the difference between previous consumption and new consumption
 * @param new_consumption_kwh
 */
void billing_processTariff(uint32_t Forward_consumption, uint32_t Backward_consumption)
{
	static uint32_t previous_consumptionKWH = 0;
	uint32_t total_consumption = abs(Forward_consumption + Backward_consumption) / 100;

	billing_currentBillingCycle.Forward_fraction = Forward_consumption % 100;
	/* TODO:: handle backward kwh */
	billing_lifetimeTotalsRegisters.Backward_KWH += Backward_consumption / 100.0;
	billing_customerTotalsRegisters.Backward_KWH += Backward_consumption / 100.0;
	billing_currentBillingCycle.Backward_KWH += Backward_consumption / 100;
	billing_currentBillingCycle.Backward_fraction = Backward_consumption % 100;

	while (previous_consumptionKWH < total_consumption)
	{
		billing_processTariff_PerKWH();
		previous_consumptionKWH++;
	}
	/*  check whether the relay should open or not */
	billing_RelayControl();
}

/**
 * @brief Apply step tariff logic
 * should be called after consuming every single KWH (whole 1 KWH)
 *
 * @param consumptionKWH Canceled, the function should be called with every 1 KWH
 */
static void billing_processTariff_PerKWH(void)
{
	billing_currentBillingCycle.Forward_KWH++; /* Increment the consumptionKWH by the added consumption in KWH */
	billing_lifetimeTotalsRegisters.Forward_KWH++;
	billing_customerTotalsRegisters.Forward_KWH++;

	/* First step, first KWH fee, should be invoked on 1st KWH (zero consumption is handled in closeBillingCycle) */
	if (billing_currentBillingCycle.Forward_KWH == 1)
	{
		applyStepFee();
	}

	if (billing_currentBillingCycle.Forward_KWH > billing_currentStepTariffPlan.steps[billing_currentBillingCycle.currentStep].toKWH)
	{

		billing_currentBillingCycle.currentStep++;

		/* First KWH in a recalculation step */
		if (billing_currentStepTariffPlan.steps[billing_currentBillingCycle.currentStep].recalculationFlag == 1)
		{

			billing_currentBillingCycle.consumptionValue = (billing_currentBillingCycle.Forward_KWH - 1) * billing_currentStepTariffPlan.steps[billing_currentBillingCycle.currentStep].stepPrice / 100.0; /* Recalculate the cycle consumption times the step price */

			/* Re-add the previous fees that have been overwritten by the recalculation */
			applyStepFeeAfterRecalculation();
		}

		/* Should be called after the recalculation logic that updates the consumption value */
		applyStepFee(); /* On step start	 */
	}

	billing_currentBillingCycle.consumptionValue += billing_currentStepTariffPlan.steps[billing_currentBillingCycle.currentStep].stepPrice / 100.0; /* Increment the consumptionValue by 1 KWH value */
	updateRemainingCredit();
}

/******************************************************************************
 Description     ::		set sandby tariff activation date
 Function        ::		billing_setTariffActivationDate
 Input           ::		month, year
 Output          ::		DLMS_ERROR_CODE error check
 Call            ::
 ******************************************************************************/
DLMS_ERROR_CODE billing_setTariffActivationDate(uint8_t month, uint16_t year)
{

	if (month == 0 || month > 12)
	{
		return DLMS_ERROR_CODE_INVALID_DATE_TIME;
	}
	else
	{
		billing_futureStepTariffPlan.activationMonth = month;
		billing_futureStepTariffPlan.activationYear = year;
		return DLMS_ERROR_CODE_OK;
	}
}

/******************************************************************************
 Description     ::		check if the time of activate come to activate new tariff
 Function        ::		billing_isTariffActivationTime
 Input           ::		none
 Output          ::		yes = 1, no = 0
 Call            ::		rtc_get_date
 ******************************************************************************/
int billing_isTariffActivationTime(void)
{
	uint8_t rtc_month;
	uint16_t rtc_year;

	rtc_get_date(&rtc_year, &rtc_month, NULL, NULL);

	if (billing_currentStepTariffPlan.activationCode >= billing_futureStepTariffPlan.activationCode)
	{
		return (0);
	}
	if (billing_currentStepTariffPlan.activationCode < billing_futureStepTariffPlan.activationCode) /* check activation code serial is correct */
	{
		if ((rtc_year > billing_futureStepTariffPlan.activationYear) ||
			(rtc_year == billing_futureStepTariffPlan.activationYear && rtc_month >= billing_futureStepTariffPlan.activationMonth)) /* check if it's activation time */
		{
			return (1);
		}
	}

	return (0);
}

/******************************************************************************
 Description     ::		copy future tariff to active tariff
 Function        ::		billing_ActivateNewTariff
 Input           ::		none
 Output          ::		none
 Call            ::		rtc_get_date, eeprom_instantaneous_save
 ******************************************************************************/
void billing_ActivateNewTariff(void)
{
	uint8_t rtc_month;
	uint16_t rtc_year;

	rtc_get_date(&rtc_year, &rtc_month, NULL, NULL);

	billing_currentStepTariffPlan = billing_futureStepTariffPlan;
	billing_currentStepTariffPlan.activationMonth = rtc_month;
	billing_currentStepTariffPlan.activationYear = rtc_year;
	 
	eeprom_instantaneous_save(EEPROM_STRUCT_BILLING_CURRENT_STEP_TARIFF_PLAN);
	eeprom_instantaneous_save(EEPROM_STRUCT_BILLING_FUTURE_STEP_TARIFF_PLAN);
}

/*=========================================================== off day APIs ======================================================================*/

/*=========================================================
 Description     ::		set weekends
 Function        ::		billing_setWeeklyOffDays
 Input           ::		new_weeklyOffDays
 Output          ::		none
 Call            ::		none
 =========================================================*/
void billing_setWeeklyOffDays(billing_WeekDays new_weeklyOffDays)
{
	billing_weekends = new_weeklyOffDays;
	force_update = 1;
	billing_updateOffDays();
}

/*=========================================================
 Description     ::		set OffDays if year, billing_holidays, weekoffdays changed
 Function        ::		billing_updateOffDays
 Input           ::		none
 Output          ::		none
 Call            ::		day_of_week, rtc_get_date
 Effect          ::		update billing_offDays object
 =========================================================*/
void billing_updateOffDays(void)
{
	uint16_t current_year;
	rtc_get_date(&current_year, NULL, NULL, NULL);

	if (billing_weekends != billing_offDays.week || current_year != billing_offDays.base_year || force_update == 1)
	{
		/*recalculate off days*/
		for (int month = 0; month < 12; month++)
		{ /**TODO:: RTC integration**/
			billing_offDays.base_month = day_of_week(month + 1, 1, (int)current_year);
			for (uint8_t week_no = 0; week_no < 5; week_no++)
			{
				billing_offDays.months[month] <<= 7;
				billing_offDays.months[month] |= WEEK_CYCLIC_SHIFT(billing_weekends, billing_offDays.base_month);
			}
			billing_offDays.months[month] |= billing_holidays.months[month];
		}
		billing_offDays.base_year = current_year;
		billing_offDays.week = billing_weekends;
		force_update = 0;
		 
		eeprom_instantaneous_save(EEPROM_STRUCT_BILLING_WEEKLENDS);
		eeprom_instantaneous_save(EEPROM_STRUCT_BILLING_HOLIDAYS);
	}
}

/*=========================================================
 Description     ::		return if the day is off or not
 Function        ::		billing_isOffDay
 Input           ::		nont
 Output          ::		0: false, 1: true
 Call            ::		none
 =========================================================*/
int billing_isOffDay(void)
{
	uint32_t day = 0;
	uint32_t month = 0;
	rtc_get_date(NULL, &month, &day, NULL);
	return Get_BIT(billing_offDays.months[month - 1], (day - 1));
}

/*=========================================================
 Description     ::		add holiday to billing_holidays object
 Function        ::		billing_AddHoliday
 Input           ::		uint8_t day,uint8_t month,uint16_t year
 Output          ::		DLMS_ERROR_CODE_OK : added successfully, DLMS_ERROR_CODE_REJECTED: different year
 Call            ::		none
 =========================================================*/
DLMS_ERROR_CODE billing_AddHoliday(uint8_t day, uint8_t month)
{
	SET_BIT(billing_holidays.months[month - 1], (day - 1));
	SET_BIT(billing_offDays.months[month - 1], (day - 1));
	force_update = 1;
	billing_updateOffDays();
	return DLMS_ERROR_CODE_OK;
}

/*=========================================================
 Description     ::		remove holiday from billing_holidays object
 Function        ::		billing_removeHoliday
 Input           ::		day, month, year
 Output          ::		DLMS_ERROR_CODE_OK : added successfully, DLMS_ERROR_CODE_REJECTED: different year
 Call            ::		none
 =========================================================*/
DLMS_ERROR_CODE billing_removeHoliday(uint8_t day, uint8_t month)
{
	month--; /* get index of the month */
	CLEAR_BIT(billing_holidays.months[month], (day - 1));
	force_update = 1;
	billing_updateOffDays();
	return DLMS_ERROR_CODE_OK;
}

/*=========================================================
 Description     ::		reset billing_holidays object
 Function        ::		billing_resetHolidays
 Input           ::		none
 Output          ::		none
 Call            ::		none
 =========================================================*/
void billing_resetHolidays(void)
{
	for (uint8_t i = 0; i < 12; i++)
	{
		billing_holidays.months[i] = 0;
	}
	force_update = 1;
	billing_updateOffDays();
}

/*=========================================================
 Description     ::		set friendly time range
 Function        ::		billing_setStartFriendlyTime
 Input           ::		start_hour, start_minutes, end_hours, end_minutes
 Output          ::		none
 Call            ::		none
 Note			 ::		time must be in 24 mode, if start time equals end time this exact minute will be the friendly time.
=========================================================*/
void billing_setStartFriendlyTime(uint8_t start_hour, uint8_t start_minutes)
{
	if (start_hour > 23 || start_minutes > 59)
	{
		return;
	}
	billing_start_friendlytime.hour = start_hour;
	billing_start_friendlytime.minute = start_minutes;

	if (billing_start_friendlytime.hour < billing_end_friendlytime.hour)
	{
		billing_friendlytime_reversed = 0;
	}
	else if (billing_start_friendlytime.hour > billing_end_friendlytime.hour)
	{
		billing_friendlytime_reversed = 1;
	}
	else /*  billing_start_friendlyhours == billing_end_friendlyhours */
	{
		if (billing_start_friendlytime.minute < billing_end_friendlytime.minute)
		{
			billing_friendlytime_reversed = 0;
		}
		else if (billing_start_friendlytime.minute > billing_end_friendlytime.minute)
		{
			billing_friendlytime_reversed = 1;
		}
		else
		{
			billing_friendlytime_reversed = 0;
		}
	}

	/* update data on EEPROM*/
	
	eeprom_instantaneous_save(EEPROM_UINT8_BILLING_START_FRIENDLY_TIME);
	eeprom_instantaneous_save(EEPROM_UINT8_BILLING_FRIENDLYTIME_REVERSED);
}

/*=========================================================
 Description     ::		set end of friendly time range
 Function        ::		billing_setEndFriendlyTime
 Input           ::		start_hour, start_minutes, end_hours, end_minutes
 Output          ::		none
 Call            ::		none
 Note			  ::		time must be in 24 mode, if start time equals end time this exact minute will be the friendly time.
 ========================================================= */
void billing_setEndFriendlyTime(uint8_t end_hours, uint8_t end_minutes)
{
	if (end_hours > 23 || end_minutes > 59)
	{
		return;
	}
	billing_end_friendlytime.hour = end_hours;
	billing_end_friendlytime.minute = end_minutes;

	if (billing_start_friendlytime.hour < billing_end_friendlytime.hour)
	{
		billing_friendlytime_reversed = 0;
	}
	else if (billing_start_friendlytime.hour > billing_end_friendlytime.hour)
	{
		billing_friendlytime_reversed = 1;
	}
	else /* billing_start_friendlyhours == billing_end_friendlyhours */
	{
		if (billing_start_friendlytime.minute < billing_end_friendlytime.minute)
		{
			billing_friendlytime_reversed = 0;
		}
		else if (billing_start_friendlytime.minute > billing_end_friendlytime.minute)
		{
			billing_friendlytime_reversed = 1;
		}
		else
		{
			billing_friendlytime_reversed = 0;
		}
	}
	/* update data on eeprom */
	 
	eeprom_instantaneous_save(EEPROM_UINT8_BILLING_END_FRIENDLY_TIME);
	eeprom_instantaneous_save(EEPROM_UINT8_BILLING_FRIENDLYTIME_REVERSED);
}

static int checkTimeInRange(uint8_t hour_mode24, uint8_t minute)
{

	if (hour_mode24 > billing_start_friendlytime.hour)
	{
		if (hour_mode24 < billing_end_friendlytime.hour)
		{
			return 1;
		}
		else if (hour_mode24 == billing_end_friendlytime.hour && minute <= billing_end_friendlytime.minute)
		{
			return 1;
		}
	}
	else if (hour_mode24 == billing_start_friendlytime.hour && minute >= billing_start_friendlytime.minute)
	{
		if (hour_mode24 < billing_end_friendlytime.hour)
		{
			return 1;
		}
		else if (hour_mode24 == billing_end_friendlytime.hour && minute <= billing_end_friendlytime.minute)
		{
			return 1;
		}
	}
	return 0;
}

static int checkTimeOutRange(uint8_t hour_mode24, uint8_t minute)
{
	if (hour_mode24 < billing_start_friendlytime.hour)
	{
		if (hour_mode24 > billing_end_friendlytime.hour)
		{
			return 0;
		}
		else if (hour_mode24 == billing_end_friendlytime.hour && minute > billing_end_friendlytime.minute)
		{
			return 0;
		}
	}
	else if (hour_mode24 == billing_start_friendlytime.hour && minute < billing_start_friendlytime.minute)
	{
		if (hour_mode24 > billing_end_friendlytime.hour)
		{
			return 0;
		}
		else if (hour_mode24 == billing_end_friendlytime.hour && minute > billing_end_friendlytime.minute)
		{
			return 0;
		}
	}
	return 1;
}

/*=========================================================
 Description     ::		check if rtc is in friendly time
 Function        ::		billing_isFriendlyTime
 Input           ::		start_hour, start_minutes, end_hours, end_minutes
 Output          ::		none
 Call            ::		rtc_get_time, checkTimeInRange, checkTimeOutRange
 =========================================================*/
int billing_isFriendlyTime(void)
{
	uint8_t hour_mode24=0;
	uint8_t minute=0;
	rtc_get_time(&hour_mode24, &minute,NULL);

	return (billing_friendlytime_reversed == 0) ? checkTimeInRange(hour_mode24, minute)
												: checkTimeOutRange(hour_mode24, minute);
}

int billing_isGraceHours(void)
{
	return 0;
}

int billing_isGracePeriod(void)
{
	return 0;
}
/* =========================================================== relay control ====================================================================== */

/**
 * @brief Connect and Disconnect Relay
 * Should be called every 1KWH of consumption,billing cycle closure,
 * charge, discharge, every minute (xx:xx:00) after isNewBillingCycle() check (new bc or setDateTime)
 */
void billing_RelayControl(void)
{

	if (billing_currentStepTariffPlan.stepFeeDeductionRule == PREPAID)
	{
		if (billing_currentBillingCycle.remainingCreditBalance > 0)
		{
			/*close relay*/
			remove_event(EVENT_NAME_CREDIT_OUT);
		}
		else if (billing_currentBillingCycle.remainingCreditBalance <= 0 && billing_isOffDay() == 0 && billing_isFriendlyTime() == 0 && billing_isGraceHours() == 0 && billing_isGracePeriod() == 0)
		{
			/*open relay*/
			set_event(EVENT_NAME_CREDIT_OUT);
		}
		else
		{
			/* keep previous state */
		}
	}
}
