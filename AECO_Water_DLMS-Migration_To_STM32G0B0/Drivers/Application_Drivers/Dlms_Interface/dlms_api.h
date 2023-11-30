#ifndef _DLMS_API_H_
#define _DLMS_API_H_

#include "cosem_obis.h"
#include "Includes/cosem.h"
#include "hdlc_rx.h"

#define DLMS_SUCCESS	0
#define DLMS_FAIL	   -1

typedef struct {
	void 			*priv;
	obis_settings_t  obis_settings;
} dlms_settings_t;

extern dlmsServerSettings dlms_HdlcInstance ; 

int32_t dlms_server_init();

int32_t dlms_handl_request(dlms_settings_t* ptr, uint8_t* frame, uint32_t size, hdlc_id_t id);

int32_t dlms_send_response(dlms_settings_t* ptr, uint8_t* frame, uint32_t size);
int32_t dlms_server_reset(dlms_settings_t* ptr);
int dlms_MainFunction();

#endif /* _DLMS_API_H_ */
