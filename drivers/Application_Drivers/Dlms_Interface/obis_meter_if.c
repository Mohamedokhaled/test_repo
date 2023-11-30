
#ifndef _OBIS_METER_IF_H_
#define _OBIS_METER_IF_H_

#include "dlms_config.h"
#include "obis_meter_if.h"

int32_t get_device_id(octet_string_t* device_id, errorcode_t *errcode) {
	//memcpy(device_id->data, DLMS_DEVICE_ID, OBJ_OCTET_STRING_MAX-1);
	*errcode = 0;
	return 0;
}
int32_t get_firmware_identifier(octet_string_t *firmware_id, errorcode_t *errcode) {
	return ERRCODE_OBJ_NOT_AVAILABLE;
}

#endif /* _OBIS_METER_IF_H_ */
