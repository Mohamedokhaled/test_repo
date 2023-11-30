 
 
#include "metrologyAPI.h"
#include "fastmath.h"
#include "billing.h"




unsigned char* GET_UINT64_REGISTER_CURRENT_MONTH_TOTAL_ACTIVE_ENERGY(int* len){
	
	static uint64_t id = 0;
	extern billing_billingCycle billing_currentBillingCycle;
	id = abs(billing_currentBillingCycle.Forward_KWH*100 + billing_currentBillingCycle.Forward_fraction%100
		 + billing_currentBillingCycle.Backward_KWH*100 + billing_currentBillingCycle.Backward_fraction%100);
	return &id;
}



