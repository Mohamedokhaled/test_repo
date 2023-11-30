/*/==================================================================*/
#ifndef TASK_H_INCLUDED
#define TASK_H_INCLUDED
#include "stdint-gcc.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TASK_Num_MAX  50

typedef enum {
	TASK_EMPTYTASK = 0,
	TASK_READ_RTC,
	TASK_OPEN_SOLENOID,
	TASK_SOLENOID_CONTROL_PROC,
	TASK_RS485_PROC,
	TASK_DLMS_PROC,
	TASK_BILLING_PROC,
	TASK_SOLENOID_ACT_PROC,
	TASK_CHECK_DISCRETES_PROC,
	TASK_EEPROM_READ_BACK,
	TASK_EEPROM_CLEAR_DATA,
	TASK_EEPROM_SAVE_LOOKUP_TABLE,
	TASK_EEPROM_DYNAMIC_SAVE,
	TASK_TEST_TIMING_100MS,
	TASK_TEST_TIMING_300MS,
	TASK_TEST_TIMING_500MS,

	} TASK_CONST;

typedef struct {

	TASK_CONST tasktype;
	uint8_t offset;
	uint8_t Buff[TASK_Num_MAX];
} TASK_Str;
/*/---------------------variable declare------------------------------*/
extern TASK_Str VTask;
/*/===================================================================*/
void InitTaskQue(void);
uint8_t PutTaskIntoQue_INT(TASK_CONST TaskID);
uint8_t PutTaskIntoQue(TASK_CONST TaskID);
uint8_t GetTaskFromQue(void);

 /**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/


#endif /* DISPLAY_H_INCLUDED */
