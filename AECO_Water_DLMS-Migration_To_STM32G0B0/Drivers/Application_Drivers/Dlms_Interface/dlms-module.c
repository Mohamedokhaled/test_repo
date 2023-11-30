
#include "sys-hal.h"
#include "sys-trace.h"
#include "cosem_obis.h"
#include "dlms_config.h"
#include "dlms_api.h"
#include "hdlc_rx.h"
#include "Includes/cosem.h"
#include "Includes/server.h"
/**TODO:: Secure Element integration **/
/*#include "secure_element/secure.h"*/
/*#include "delay.h"*/

/* HDLC_BUFFER_SIZE: more than PDU_BUFFER_SIZE by 22 */
#define HDLC_BUFFER_SIZE 412
 /* PDU_BUFFER_SIZE : must be bigger than the buffersize to be sent */
#define PDU_BUFFER_SIZE 390
/* unsigned char snframeBuff[HDLC_BUFFER_SIZE + HDLC_HEADER_SIZE]; */
/* unsigned char snpduBuff[PDU_BUFFER_SIZE]; */
unsigned char lnframeBuff[HDLC_BUFFER_SIZE ];
unsigned char lnpduBuff[PDU_BUFFER_SIZE];
/* unsigned char sn47pduBuff[PDU_BUFFER_SIZE]; */
/* unsigned char ln47pduBuff[PDU_BUFFER_SIZE]; */

gxByteBuffer request ;
gxByteBuffer reply;
/* dlms_settings_t dlms_HdlcInstance ; */
dlmsServerSettings dlms_HdlcInstance ;


typedef enum{
	NORMAL,
	CALIBRATION
}
OPTICAL_OP_MODE;


extern volatile OPTICAL_OP_MODE optical_mode;


int32_t dlms_server_init() {
	
	
  /*Initialize DLMS settings.
	We have several server that are using same objects. Just copy them.
	oa_copy(&lnHdlc.settings.base.objects, &snHdlc.settings.base.objects);
	we need to increase the size to handle the variant clear , it was require to set data of unit64 in union was set with unit8*/
	sysal_init (HDLC_BUFFER_SIZE+100); /* HDLC_BUFFER_SIZE+constant : should be power of 2*/

	bb_init(&request);
	bb_init(&reply);
	int ret = bb_capacity(&request,  HDLC_BUFFER_SIZE);
	if(ret != DLMS_SUCCESS){
		/* unhandled error */
		return (ret);
	}
	ret = bb_capacity(&reply,  HDLC_BUFFER_SIZE);
	if(ret != DLMS_SUCCESS){
		/* unhandled error */
		return (ret);
	}
	svr_init(&dlms_HdlcInstance, 1 /*Logical name*/, DLMS_INTERFACE_TYPE_HDLC, 399, PDU_BUFFER_SIZE, lnframeBuff,  HDLC_BUFFER_SIZE, lnpduBuff, PDU_BUFFER_SIZE);
	/* oa_copy(&dlms_HdlcInstance.base.objects); */
	
	init_obis(&dlms_HdlcInstance);
	dlms_HdlcInstance.pushClientAddress = 145;
	svr_initialize(&dlms_HdlcInstance);

	/**secure element integartion **/
	/*uint8_t randomnum=getRandom();*/
	 uint8_t randomnum = 0;
	dlms_HdlcInstance.base.cipher.invocationCounter = randomnum;

	return (DLMS_SUCCESS);
}

int dlms_MainFunction(void)
{


	if( hdlc_rx_get_frame_ready(HDLC_OPTICAL) == 1)
	{
		dlms_handl_request( &dlms_HdlcInstance , hdlc_Optical_proc_context.buf , hdlc_Optical_proc_context.frame_length,HDLC_OPTICAL);
		hdlc_rx_set_frame_consumed(&hdlc_Optical_proc_context);
	}

	if( hdlc_rx_get_frame_ready(HDLC_RS485) == 1)
	{
		dlms_handl_request( &dlms_HdlcInstance , hdlc_RS485_proc_context.buf , hdlc_RS485_proc_context.frame_length,HDLC_RS485);
		hdlc_rx_set_frame_consumed(&hdlc_RS485_proc_context);		
	}
	if( hdlc_rx_get_frame_ready(HDLC_GSM_4G) == 1)
	{
		dlms_handl_request( &dlms_HdlcInstance , hdlc_GSM_4G_proc_context.buf , hdlc_GSM_4G_proc_context.frame_length,HDLC_GSM_4G);
		hdlc_rx_set_frame_consumed(&hdlc_GSM_4G_proc_context);
	}
	svr_monitorAll(&dlms_HdlcInstance);
  hdlc_rx_timeout_handle();
}



int32_t dlms_handl_request(dlms_settings_t* ptr, uint8_t* frame, uint32_t size,hdlc_id_t id)
{
	
	/*//gxByteBuffer bb, reply;
	//Get buffer data
	//bb_init(&bb);
	//bb_init(&reply);
	//bb_capacity(&bb, size);
	//bb_capacity(&reply, size);*/
	int ret = 0 ;
	static uint8_t count = 0 ;
	
	request.data = frame;
	request.size = size ;
	if (count > 0 )
	{
		ret = svr_handleRequest(&dlms_HdlcInstance, &request, &reply);
	}
	else
	{
		ret = svr_handleRequest(&dlms_HdlcInstance, &request, &reply);
	}
	count ++ ;
	/*//if (ret != 0)
	//{*/
	if (reply.size != 0)
	{
		/*//send reply message via serial port*/
		hdlc_Tx_proc(reply.data , reply.size,id);
	}
	else
	{
		/*//Todo Handle Error*/
		return (DLMS_FAIL);
	}
/*	//}*/
	bb_clear(&request);
	bb_clear(&reply);

	return (DLMS_SUCCESS);
}


