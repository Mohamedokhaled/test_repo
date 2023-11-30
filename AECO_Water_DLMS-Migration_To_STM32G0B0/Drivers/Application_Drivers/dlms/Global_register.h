/*
* global_register.h
*
* Created: 1/12/2023 6:11:55 PM
*  Author: Ahmed Ali
*/


#ifndef GLOBAL_REGISTER_H_
#define GLOBAL_REGISTER_H_



#include "../Dlms_Interface/dlms_api.h"
#include "object_List.h"
#include "MCU_interface/metrologyAPI.h"

static gxRegister globalRegister;

#define max_no_of_registers  1

typedef struct {
	unsigned char obis[6];
	DLMS_DATA_TYPE DDT;
	int unit;
	int scaler;
	unsigned char* (*read_api)(int*);
} obj_register;


int init_arra_register(obj_register * arr_obj_register);

int Init_all_register(gxRegister AllRegister[]);


DLMS_ERROR_CODE handle_register_read_callback(gxRegister* AllRegister,unsigned char * logicalName ,char index ,gxByteBuffer*  byteArr,DLMS_DATA_TYPE* vt);

#endif /* GLOBAL_REGISTER_H_ */