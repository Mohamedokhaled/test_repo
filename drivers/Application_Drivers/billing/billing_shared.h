/*
 * billing_shared.h
 *
 * Created: 8/1/2023 5:57:28 PM
 *  Author: Youssef
 */ 


#ifndef BILLING_SHARED_H_
#define BILLING_SHARED_H_
#include "stdint.h"

/* extern variables to be saved */
extern billing_totalsRegisters billing_lifetimeTotalsRegisters;
extern billing_totalsRegisters billing_customerTotalsRegisters;

extern billing_stepTariffPlan billing_currentStepTariffPlan;
extern billing_stepTariffPlan billing_futureStepTariffPlan;

extern billing_billingCycle billing_currentBillingCycle;
extern billing_billingCycle billing_billingCyclesHistory[NO_OF_BILLING_CYCLES];

extern billing_calendar billing_offDays;
extern billing_calendar billing_holidays;
extern billing_WeekDays billing_weekends;

extern billing_friendlytime_t billing_start_friendlytime;
extern billing_friendlytime_t billing_end_friendlytime;
extern uint8_t billing_friendlytime_reversed;



#endif /* BILLING_SHARED_H_ */