/*
* Relay.c
*
* Created: 1/19/2021 10:52:21 AM
*  Author: sara
*/
#include "solenoid.h"
#include "RTC_Wrapper/rtc_wrapper.h"
#include "utilities.h"
#include "main.h"
#include "gpio.h"
#include "eeprom.h"


/* #define SOLENOID_DEBUG */

/** TODO:: handle timer and EEPROM**/
/*
#include "Timer.h"
#include "../e2prom/eeprom.h"
*/
solenoid_t solenoid_control ={
		.status =SOLENOID_STATUS_ON,
		.operation_state= SOLENOID_IDLE,
		.enable=DISABLE,
		.timeout =0,
};



solenoid_t solenoid_control;

event_t _event[EVENTS_TOTAL_NUMBER];

status_t dataStatus;
config_t dataConfig;
cmp_t cmp;


/*ptr to callback functions*/
static void (* const relay_ptr_remoteReset_callbacks[EVENTS_ARRAY_NUM])(void) =
{
	NULL,									/* CurrentReverseL1     */  			/*bit 0	*/
	NULL,									/* CurrentReverseL2		*/				/*bit 1	*/
	NULL,									/* CurrentReverseL3		*/				/*bit 2	*/
	NULL,									/* LowBattery			*/				/*bit 3	*/
	NULL,									/* BatteryRemoved		*/				/*bit 4	*/
	NULL,									/* PowerUnbalance		*/				/*bit 5	*/
	NULL,									/* NeutralCurrentZero	*/				/*bit 6	*/
	NULL,									/* ExtMagneticInfluence	*/				/*bit 7	*/
	NULL,									/* UpperCoverOpen		*/				/*bit 8	*/
	NULL,									/* TerminalCoverOpen	*/				/*bit 9	*/
	NULL,									/* LoadOver				*/				/*bit 10*/
	NULL,									/* CreditOut			*/				/*bit 11*/
	NULL,									/* MissingPhaseVoltageL1*/				/*bit 12*/
	NULL,									/* MissingPhaseVoltageL2*/				/*bit 13*/
	NULL,									/* MissingPhaseVoltageL3*/				/*bit 14*/
	NULL,									/* SunRais_SunSet		*/				/*bit 15*/
	NULL,									/* RemoteControl		*/				/*bit 16*/
	NULL,									/* ConnectForce			*/				/*bit 17*/
	NULL,									/* DisconnectForce		*/				/*bit 18*/
	NULL,									/* PhasesFailure		*/				/*bit 19*/
	NULL,									/* CurrentOver			*/				/*bit 20*/
	NULL,									/* CurrentLoss			*/				/*bit 21*/
	NULL,									/* VoltageLoss			*/				/*bit 22*/
};


void activate_pullup(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = MOT_FB_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(MOT_FB_GPIO_Port, &GPIO_InitStruct);
}
void activate_pulldown(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = MOT_FB_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(MOT_FB_GPIO_Port, &GPIO_InitStruct);
}
void solenoid_init(void)
{
	 __HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.Pin = MOT_C1_Pin|MOT_C2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  /*Configure GPIO pin : PtPin */
  GPIO_InitStruct.Pin = MOT_FB_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT; 
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(MOT_FB_GPIO_Port, &GPIO_InitStruct);

/* EXTI interrupt initiation */
/*  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);*/
}
/**
 * @brief 
 * transition from close to open (LOW to HIGH). 
 */
void solenoid_open(void)
{
	/*TODO:: handle solenoid open and close*/
	HAL_GPIO_WritePin(GPIOB, MOT_C1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, MOT_C2_Pin, GPIO_PIN_SET);
	activate_pulldown();
	/* call timer0 to calculate 60 ms*/
	/* TODO:: handle timer integration */
	/*Timer0_Relay_start();*/
	solenoid_control.operation_state = SOLENOID_FEED_BACK_CHECKS;
	solenoid_control.required_status = SOLENOID_STATUS_ON;

}
/**
 * @brief 
 * transition from open to close (HIGH to LOW). 
 */
void solenoid_close(void)
{
	/*TODO:: handle solenoid open and close*/
	HAL_GPIO_WritePin(GPIOB, MOT_C2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, MOT_C1_Pin, GPIO_PIN_SET);
	activate_pullup();
	/* call timer0 to calculate 60 ms*/
	/* TODO:: handle timer integration */
	/*Timer0_Relay_start();*/
	solenoid_control.operation_state = SOLENOID_FEED_BACK_CHECKS;
	solenoid_control.required_status = SOLENOID_STATUS_OFF;
}

void solenoid_brake(void)
{
	HAL_GPIO_WritePin(GPIOB, MOT_C1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, MOT_C2_Pin, GPIO_PIN_SET);
	solenoid_control.timeout =3;
	solenoid_control.operation_state = SOLENOID_IDLE;
}

void soleniod_idle(void)
{

	HAL_GPIO_WritePin(GPIOB, MOT_C1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, MOT_C2_Pin, GPIO_PIN_RESET);
	/* if(0 == (--solenoid_control.timeout))
	{
		HAL_GPIO_WritePin(GPIOB, MOT_C1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, MOT_C2_Pin, GPIO_PIN_RESET);
		solenoid_control.operation_state =SOLENOID_NO_OPERATION;
		
#ifdef SOLENOID_DEBUG
		printmsg("timeout done\r\n");
#endif
	} */
	if (solenoid_control.status != solenoid_control.required_status) {
		 switch(solenoid_control.status)
		 {
			case SOLENOID_STATUS_ON:
				solenoid_control.operation_state = SOLENOID_CLOSE;
#ifdef SOLENOID_DEBUG
			printmsg("solenoid close selected\r\n");
#endif
				break;
			case SOLENOID_STATUS_OFF:
				solenoid_control.operation_state = SOLENOID_OPEN;
#ifdef SOLENOID_DEBUG
			printmsg("solenoid open selected\r\n");
#endif
				break;
		 }
	}
}
/**
 * procedure :
 * 1. due to the required pin status configure the corresponding pull/up or pull/down
 * 2. check FB pin status
 * 3. check operation state and move forward to action
*/
void solenoid_FB_check(void)
{	/** step 1 **/
	if (SOLENOID_STATUS_OFF == solenoid_control.required_status)
	{
		activate_pullup();
		if(GPIO_PIN_RESET == HAL_GPIO_ReadPin(GPIOB,MOT_FB_Pin))
		{
			solenoid_control.status= SOLENOID_STATUS_OFF;
			/* solenoid_control.operation_state = SOLENOID_ELEC_BRAKE ; */
			solenoid_control.operation_state = SOLENOID_IDLE ;
#ifdef SOLENOID_DEBUG
		printmsg("solenoid feed back close selected   !\r\n");
#endif
		}
	}
	else
	{
		activate_pulldown();
		if(HAL_GPIO_ReadPin(GPIOB,MOT_FB_Pin)==GPIO_PIN_SET)
		{
			solenoid_control.status= SOLENOID_STATUS_ON;
			/* solenoid_control.operation_state = SOLENOID_ELEC_BRAKE ; */
			solenoid_control.operation_state = SOLENOID_IDLE ;
#ifdef SOLENOID_DEBUG
		printmsg("solenoid feed back open selected  !\r\n");
#endif
		} 
	}
	
}



void solenoid_control_proc(void)
{

	if (0xFF == solenoid_control.timeout ) {
		solenoid_control.timeout=0;
	}
	switch (solenoid_control.operation_state)
	{
		case SOLENOID_OPEN:
			solenoid_open();
#ifdef SOLENOID_DEBUG
			printmsg("solenoid open \r\n");
#endif
			break;
		case SOLENOID_CLOSE:
			solenoid_close();
#ifdef SOLENOID_DEBUG
			printmsg("solenoid close \r\n");
#endif
			break;
		case SOLENOID_FEED_BACK_CHECKS:
#ifdef SOLENOID_DEBUG
			solenoid_FB_check();
			printmsg("solenoid feedback \r\n");
#endif
			break;
		case SOLENOID_ELEC_BRAKE:
			solenoid_brake();
#ifdef SOLENOID_DEBUG
			printmsg("solenoid brake \r\n");
#endif
			break;
		case SOLENOID_IDLE:
			soleniod_idle();
#ifdef SOLENOID_DEBUG
			printmsg("solenoid idle \r\n");
#endif
			break;
		default:
			break;
	}
}


void solenoid_feedback_callback(solenoid_operation_state_t state)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	switch (state)
	{
		case SOLENOID_OPEN:
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13,GPIO_PIN_SET);
			/*HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_13);*/
			GPIO_InitStruct.Pull = GPIO_PULLUP;
			GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
			GPIO_InitStruct.Pin = MOT_FB_Pin;
			HAL_GPIO_Init(MOT_FB_GPIO_Port, &GPIO_InitStruct);


			break;
		case SOLENOID_CLOSE:
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13,GPIO_PIN_SET);
			GPIO_InitStruct.Pull = GPIO_PULLDOWN;
			GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
			GPIO_InitStruct.Pin = MOT_FB_Pin;
			HAL_GPIO_Init(MOT_FB_GPIO_Port, &GPIO_InitStruct);
			break;

		default:
			break;
	}


}



void init_event(void){
	_event->DateTime.day = 0;
	_event->DateTime.month = 0;
	_event->DateTime.year = 0;
	_event->DateTime.hour = 0;
	_event->DateTime.min = 0;
	_event->DateTime.sec = 0;
	_event->count =0;
}

/* set event date and time function*/
void set_event(uint8_t eventName){
	if(_event[eventName].DateTime.year==0){
		/* save date and time of required event */
		/** TODO:: RTC integration**/
		/*rtc_get_date(RTC,&_event[eventName].DateTime.year,&_event[eventName].DateTime.month,&_event[eventName].DateTime.day,NULL);
		rtc_get_time(RTC,&_event[eventName].DateTime.hour,&_event[eventName].DateTime.min,&_event[eventName].DateTime.sec);*/
		rtc_get_date(&_event[eventName].DateTime.year, &_event[eventName].DateTime.month, &_event[eventName].DateTime.day, NULL);
		rtc_get_time(&_event[eventName].DateTime.hour,&_event[eventName].DateTime.min,&_event[eventName].DateTime.sec);
		/* according to the events related to relay control tampers status we set the status flag if required*/
		switch(eventName){
			case EVENT_NAME_CURRENT_REVERSE_L1: dataStatus.bits.CurrentReverseL1 = 1;break;
			case EVENT_NAME_CURRENT_REVERSE_L2: dataStatus.bits.CurrentReverseL2 = 1;break;
			case EVENT_NAME_CURRENT_REVERSE_L3: dataStatus.bits.CurrentReverseL3 = 1;break;
			case EVENT_NAME_LOW_BATTARY: dataStatus.bits.LowBattery = 1;break;
			case EVENT_NAME_BATTARY_REMOVED: dataStatus.bits.BatteryRemoved = 1;break;
			case EVENT_NAME_POWER_UNBALANCE: dataStatus.bits.PowerUnbalance = 1;break;
			case EVENT_NAME_NEUTRAL_CURRENT_ZERO: dataStatus.bits.NeutralCurrentZero = 1;break;
			case EVENT_NAME_EXT_MAGNETIC_INFLUENCE: dataStatus.bits.ExtMagneticInfluence = 1;break;
			case EVENT_NAME_UPPER_COVER_OPEN:dataStatus.bits.UpperCoverOpen = 1;break;
			case EVENT_NAME_TERMINAL_COVER_OPEN:dataStatus.bits.TerminalCoverOpen = 1;break;
			case EVENT_NAME_POWER_LOAD_OVER:dataStatus.bits.LoadOver = 1;break;
			case EVENT_NAME_CREDIT_OUT:dataStatus.bits.CreditOut = 1;break;
			case EVENT_NAME_MISSING_PHASE_VOLTAGE_L1:dataStatus.bits.MissingPhaseVoltageL1 = 1;break;
			case EVENT_NAME_MISSING_PHASE_VOLTAGE_L2:dataStatus.bits.MissingPhaseVoltageL2 = 1;break;
			case EVENT_NAME_MISSING_PHASE_VOLTAGE_L3:dataStatus.bits.MissingPhaseVoltageL3 = 1;break;
			case EVENT_NAME_PHASE_FAILURE:dataStatus.bits.PhasesFailure = 1;break;
			case EVENT_NAME_REMOTE_CONTROL: dataStatus.bits.RemoteControl = 1;break;/*  Disconnect*/
			case EVENT_NAME_FORCE_CONNECT: dataStatus.bits.ConnectForce = 1;break;
			case EVENT_NAME_FORCE_DISCONNECT: dataStatus.bits.DisconnectForce= 1;break;
			case EVENT_NAME_SUN_RAISE_SUN_SET : dataStatus.bits.SunRais_SunSet = 1;break;
			case EVENT_NAME_CURRENT_LOAD_OVER:dataStatus.bits.CurrentOver = 1;break;
			case EVENT_NAME_CURRENT_LOSS:dataStatus.bits.CurrentLoss = 1;break;
			case EVENT_NAME_VOLTAGE_LOSS:dataStatus.bits.VoltageLoss = 1;break;
		}
		_event[eventName].count++;
		
		if(eventName<32 && eventName != EVENT_NAME_SUN_RAISE_SUN_SET)
		{
			/** TODO:: EEPROM integration**/

			eeprom_instantaneous_save(EEPROM_STRUCT_RELAY_DATA_STATUS);
		}
		eeprom_instantaneous_save(EEPROM_ARRAY_RELAY_EVENTS);
	}

}

/* get Date time Data for specific event*/
event_t get_event(uint8_t eventName){
	
	return _event[eventName];
}



void remove_event(uint8_t eventName){
	/* remove date and time of required event */
	_event[eventName].DateTime.day=0;
	_event[eventName].DateTime.month=0;
	_event[eventName].DateTime.year=0;
	_event[eventName].DateTime.hour=0;
	_event[eventName].DateTime.min=0;
	_event[eventName].DateTime.sec=0;
	/* according to the events related to relay control tampers status we reset the status flag if required*/
	switch(eventName){
		case EVENT_NAME_CURRENT_REVERSE_L1: dataStatus.bits.CurrentReverseL1=0;break;
		case EVENT_NAME_CURRENT_REVERSE_L2: dataStatus.bits.CurrentReverseL2=0;break;
		case EVENT_NAME_CURRENT_REVERSE_L3: dataStatus.bits.CurrentReverseL3=0;break;
		case EVENT_NAME_LOW_BATTARY: dataStatus.bits.LowBattery=0;break;
		case EVENT_NAME_BATTARY_REMOVED: dataStatus.bits.BatteryRemoved = 0;break;
		case EVENT_NAME_POWER_UNBALANCE: dataStatus.bits.PowerUnbalance=0;break;
		case EVENT_NAME_NEUTRAL_CURRENT_ZERO: dataStatus.bits.NeutralCurrentZero=0;break;
		case EVENT_NAME_EXT_MAGNETIC_INFLUENCE: dataStatus.bits.ExtMagneticInfluence=0;break;
		case EVENT_NAME_UPPER_COVER_OPEN:dataStatus.bits.UpperCoverOpen=0;break;
		case EVENT_NAME_TERMINAL_COVER_OPEN:dataStatus.bits.TerminalCoverOpen=0;break;
		case EVENT_NAME_POWER_LOAD_OVER:dataStatus.bits.LoadOver=0;break;
		case EVENT_NAME_CREDIT_OUT:dataStatus.bits.CreditOut=0;break;
		case EVENT_NAME_MISSING_PHASE_VOLTAGE_L1:dataStatus.bits.MissingPhaseVoltageL1=0;break;
		case EVENT_NAME_MISSING_PHASE_VOLTAGE_L2:dataStatus.bits.MissingPhaseVoltageL2=0;break;
		case EVENT_NAME_MISSING_PHASE_VOLTAGE_L3:dataStatus.bits.MissingPhaseVoltageL3=0;break;
		case EVENT_NAME_SUN_RAISE_SUN_SET:dataStatus.bits.SunRais_SunSet=0;break;
		case EVENT_NAME_PHASE_FAILURE:dataStatus.bits.PhasesFailure=0;break;
    	case EVENT_NAME_REMOTE_CONTROL: dataStatus.bits.RemoteControl = 0;break; /* Reconnect*/
		case EVENT_NAME_FORCE_CONNECT: dataStatus.bits.ConnectForce = 0;break;
		case EVENT_NAME_FORCE_DISCONNECT: dataStatus.bits.DisconnectForce = 0;break;
		case EVENT_NAME_CURRENT_LOAD_OVER:dataStatus.bits.CurrentOver=0;break;
		case EVENT_NAME_CURRENT_LOSS:dataStatus.bits.CurrentLoss=0;break;
		case EVENT_NAME_VOLTAGE_LOSS:dataStatus.bits.VoltageLoss=0;break;
	
	}
	if(eventName<32 && eventName != EVENT_NAME_SUN_RAISE_SUN_SET)
	{
		/**TODO:: EEPROM integration**/
		eeprom_instantaneous_save(EEPROM_STRUCT_RELAY_DATA_STATUS);
	}
	eeprom_instantaneous_save(EEPROM_ARRAY_RELAY_EVENTS);
}

/* remove event and call the remote callback function */
DLMS_ERROR_CODE relay_RemoteEventReset(uint8_t eventName)
{
	if (eventName >= EVENTS_ARRAY_NUM)
	{
		return DLMS_ERROR_CODE_INVALID_COMMAND;
	}
	/**TODO:: remote reset solenoid events callback **/
	if(relay_ptr_remoteReset_callbacks[eventName] != NULL)
	{
		relay_ptr_remoteReset_callbacks[eventName]();
	}
	remove_event(eventName);
	return DLMS_ERROR_CODE_OK;
}



/*
compare Meter data configuration and meter data status matching
results if any of parameters is matched to one open the relay
this function will bes called in an individual task in normal mode
*/

void solenoid_status_config_cmp_act(void){
	cmp.data = dataStatus.data&dataConfig.data;
	/*
	handling remote connection with sunSet/sunRais
	the priority as following:
	1- Tampers.
	2- Remote Connect
	3- Sun set/raise  event.
	
	*/
	/* connect
	 check on sunset/raise bit status = 1 , RemoteControl bit status = 0 , connect flag = 1 , disconnect flag = 0*/
	if((cmp.bits.CurrentReverseL1==0)&&(cmp.bits.CurrentReverseL2==0)&&(cmp.bits.CurrentReverseL3==0)&&(cmp.bits.LowBattery==0)
	&&(cmp.bits.PowerUnbalance==0)&&(cmp.bits.NeutralCurrentZero==0)&&(cmp.bits.ExtMagneticInfluence==0)&&(cmp.bits.UpperCoverOpen==0)
	&&(cmp.bits.TerminalCoverOpen==0)&&(cmp.bits.LoadOver==0)&&(cmp.bits.CreditOut==0)&&(cmp.bits.MissingPhaseVoltageL1==0)&&
	(cmp.bits.MissingPhaseVoltageL2==0)&&(cmp.bits.MissingPhaseVoltageL3==0)&&(cmp.bits.PhasesFailure==0)&&(cmp.bits.CurrentOver==0)
	&& (cmp.bits.VoltageLoss==0)){
		if((dataStatus.bits.ConnectForce==1)&&(dataStatus.bits.DisconnectForce==0)&&(dataStatus.bits.RemoteControl==0)&&
		(dataConfig.bits.SunRais_SunSet==1)&&(dataConfig.bits.ConnectForce==1)&&(dataConfig.bits.DisconnectForce==1)){
			cmp.data = 0;
		}
	}
	/* disconnect
	 check on sunset/raise bit status = 1 , RemoteControl bit status = 1 , disconnect flag = 1 , connect flag = 0*/
	 if((dataStatus.bits.ConnectForce==0)&&(dataStatus.bits.DisconnectForce==1)&&(dataStatus.bits.RemoteControl==1)
	&&(dataConfig.bits.SunRais_SunSet==1)&&(dataConfig.bits.ConnectForce==1)&&(dataConfig.bits.DisconnectForce==1)){
		cmp.data = 1;
	}
	if(cmp.data == 0){
		solenoid_control.required_status = SOLENOID_STATUS_ON;
	}
	else{
		
		solenoid_control.required_status = SOLENOID_STATUS_OFF;
	}
}

/*** TODO:: Handle tampers GPIO interrupts***/

