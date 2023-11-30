/*
 * global_register.c
 *
 * Created: 3/23/2023 10:08:20 AM
 *  Author: asamir
 */ 

#include "global_register.h"

int init_arra_register(obj_register * arr_obj_register){
	
	//	obj_register arr_obj_register[max_no_of_registers];
	// --------------	
	
	arr_obj_register[REGISTER_CURRENT_MONTH_TOTAL_ACTIVE_ENERGY] = ( obj_register) {
		.obis = {1,0,15,8,0,255},
		.DDT = DLMS_DATA_TYPE_UINT64,
		.unit = DLMS_UNIT_ACTIVE_ENERGY,
		.scaler = -2 ,
	.read_api = &GET_UINT64_REGISTER_CURRENT_MONTH_TOTAL_ACTIVE_ENERGY};
	
	

	return 1;
}

int Init_all_register(gxRegister AllRegister[]){
	obj_register arr_obj_register[max_no_of_registers];
	init_arra_register( arr_obj_register);
	int ret;

	for (int req_Obj_index = 0; req_Obj_index < max_no_of_registers; req_Obj_index++)
	{
		
		gxRegister globalRegister;
		const unsigned char* ln = arr_obj_register[req_Obj_index].obis;
		ret = cosem_init2((gxObject *)&globalRegister, DLMS_OBJECT_TYPE_REGISTER, ln);
		globalRegister.value.vt = arr_obj_register[req_Obj_index].DDT;
		globalRegister.read_api=arr_obj_register[req_Obj_index].read_api;
		if (ret == 0)
		{
			globalRegister.unit = arr_obj_register[req_Obj_index].unit;
			globalRegister.scaler = arr_obj_register[req_Obj_index].scaler;
		}
		else
		{
			break;
		}
		AllRegister[req_Obj_index] = globalRegister;
	}
	 

	return ret;
}

DLMS_ERROR_CODE handle_register_read_callback(gxRegister* AllRegister,unsigned char * logicalName ,char index ,gxByteBuffer*  byteArr,DLMS_DATA_TYPE* vt){
	int len=0;
	for (int rg = 0; rg < max_no_of_registers; rg++)
	{
		
		int cpresult = memcmp(logicalName, AllRegister[rg].base.logicalName, 6);
		
		if (cpresult == 0)
		{
			if(index ==3){ //handle scalar attribute 
				
				*vt = DLMS_DATA_TYPE_UINT8;
				return cosem_getUInt8(byteArr, AllRegister[rg].scaler);
				
			}
			else if(index ==2) // handle value attribute
			{
				
				*vt = AllRegister[rg].value.vt;
				switch (AllRegister[rg].value.vt)
				{
					case DLMS_DATA_TYPE_UINT64:
					{
						int return_v =  cosem_setUInt64(byteArr,(uint64_t *)AllRegister[rg].read_api(&len));
						return return_v;
					}
					
					break;
					case DLMS_DATA_TYPE_UINT32:
					{
						int return_v =cosem_setUInt32(byteArr,*(uint32_t *) AllRegister[rg].read_api(&len));
						return return_v;
					}
					break;
					case DLMS_DATA_TYPE_UINT16:
					{
						return  cosem_setUInt16(byteArr,*(uint16_t *)AllRegister[rg].read_api(&len));
					}
					break;
					case DLMS_DATA_TYPE_UINT8:
					{
						return cosem_setUInt8(byteArr, *(uint8_t *)AllRegister[rg].read_api(&len));
					}
					break;
					default :
					
					break;
					
				}///end of switch
				
			}//end of attr2
			return 1; // i found what i need
		} //end of if
		
	} //loop for all obj
	return 0;
}//fun
