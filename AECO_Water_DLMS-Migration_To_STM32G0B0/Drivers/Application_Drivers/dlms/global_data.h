/*
 * global_data.h
 *
 * Created: 3/28/2023 7:04:45 PM
 *  Author: asamir
 */ 


#ifndef GLOBAL_DATA_H_
#define GLOBAL_DATA_H_


#include "../Dlms_Interface/dlms_api.h"
#include "object_List.h"
#include "MCU_interface/metrologyAPI.h"

static gxData globaldata;

#define max_no_of_data  58

typedef struct {
	unsigned char obis[6];
	DLMS_DATA_TYPE DDT; 
	unsigned char* (*read_api)(int*);
	DLMS_ERROR_CODE (*write_api)(unsigned char*,int*);
	uint8_t access_mode [4];
} obj_data;


int _init_arra_data(obj_data * arr_obj);

int init_all_data(gxData AllData[]);


DLMS_ERROR_CODE handle_data_read_callback(gxData* Alldata,unsigned char * logicalName ,char index ,gxByteBuffer*  byteArr,DLMS_DATA_TYPE* vt);
DLMS_ERROR_CODE handle_data_write_callback(gxData* Alldata,unsigned char* logicalName ,char index ,dlmsVARIANT*  byteArr);


#endif /* GLOBAL_DATA_H_ */
