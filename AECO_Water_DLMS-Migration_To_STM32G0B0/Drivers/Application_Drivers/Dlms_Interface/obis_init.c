
#include "cosem_obis.h"
#include "dlms_config.h"
#include "sys-hal.h"
#include "sys-trace.h"
#include "obis_meter_if.h"
#include "Includes/cosem.h"

#include "Includes/converters.h"
#include "Includes/helpers.h"
#include "Includes/date.h"
#include "Includes/gxkey.h"
#include "Includes/gxobjects.h"
#include "Includes/gxget.h"
#include "Includes/gxset.h"
/*  Add this if you want to send notify messages. */
#include "Includes/notify.h"
 /* Add support for serialization. */
#include "Includes/gxserializer.h"
#include "solenoid.h"

#include "object_List.h"
#include "../dlms/global_data.h"
#include "../dlms/Global_register.h"

#include "dlms/Global_register.h"
#include "BaseAPI.h"

#include "Image.h"

#include "RTC_Wrapper/rtc_wrapper.h"


#define OBIS_LOGICAL_IEC_HDLC                             { 0, 0, 22, 0, 0, 255 }
#define OBIS_LOGICAL_CLOCK	                              { 0, 0,  1, 0, 0, 255 }
/* todo Marwan : add date and time obis new date and time objects */
#define  OBIS_LOGICAL_NAME_TIME							{0,0,1,0,1,255}
#define  OBIS_LOGICAL_NAME_DATE							{0,0,1,0,2,255}


#define OBIS_LOGICAL_NAME_ASSOCIATION_PUBLIC		      { 0, 0, 40, 0, 1, 255 }
#define OBIS_LOGICAL_NAME_ASSOCIATION_RO 				  { 0, 0, 40, 0, 2, 255 }
#define OBIS_LOGICAL_NAME_ASSOCIATION_RW				  { 0, 0, 40, 0, 3, 255 }
#define OBIS_LOGICAL_NAME_ASSOCIATION_FW 				  { 0, 0, 40, 0, 4, 255 }
#define OBIS_EVENT_LOG                                    { 1, 0, 99, 98, 0, 255 }
#define OBIS_SAP_LIST                                     { 0, 0, 41, 0, 0, 255 }
#define OBIS_RELAY_CONNECTION_CONTROL                     { 0, 0, 96, 3, 10, 255}
#define OBIS_METER_CREDIT                                 { 0 , 0 , 19 ,10 ,106 ,255}
#define OBIS_METER_CHARGE                                 { 0 , 0 , 19 ,20 ,106 ,255}

#define OBIS_IMAGE_TRANSFER                               { 0 , 0 , 44 ,0 ,0 ,255}

#define SERVER_ADDR						0x91
#define CLIENT_PUBLIC					0x01
#define CLIENT_RO						0x02 /*  read only */
#define CLIENT_RW						0x64 /*  reader writer admin */
#define CLIENT_FW						0x03 /*  firmware update client */


static gxIecHdlcSetup hdlc;
gxClock clock1;
/* todo Marwan :add date and time obis new date and time objects */
gxClock clock_time;
gxClock clock_date;

/*//--------------------------------------------------------------*/
static gxAssociationLogicalName associationPublic;
static gxAssociationLogicalName associationReadOnly;
static gxAssociationLogicalName associationReadWriter;
static gxAssociationLogicalName associationFW;
static gxProfileGeneric eventLog;
static gxSapAssignment sapAssignment;
 gxRegister AllRegister[max_no_of_registers];
 gxData AllData[max_no_of_data];
static gxDisconnectControl disconnectControl;
/*  static gxCredit metercredit; // todo check class charge - missing blue book edition 13
	static gxCharge metercharge; // todo check class charge - missing blue book edition 13
	Security Setup HighGMac is for GMac authentication. */
static gxSecuritySetup securitySetupHighGMac;

gxImageTransfer imageTransfer;
gxImageActivateInfo IMAGE_ACTIVATE_INFO[1];
time_t imageActionStartTime = 0;
/**system title**/
static unsigned char SERVER_SYSTEM_TITLE[8] ="AECOSRVR";
/* Define client system title. */
static unsigned char CLIENT_SYSTEM_TITLE[8] = "AECODLMS";

#pragma region client setup

static gxObject *ASS_NO_SEC_OBJECTS[] = {
	BASE(AllData[DATA_INVOCATION_COUNTER]),
	BASE(AllData[DATA_METER_SERIAL]),
	BASE(AllData[DATA_DEVICE_ID]),
	BASE(hdlc),
	BASE(clock1),
	/* todo Marwan :add date and time obis new date and time objects */
	BASE(clock_time),
	BASE(clock_date),
	 
	BASE(associationReadWriter),
	BASE(associationPublic)
};



static gxObject *ASS_LOW_SEC_OBJECTS[max_no_of_data + max_no_of_registers + 8] = {};


int addImageTransfer()
{
	int ret;
	static unsigned char IMAGE_TRANSFERRED_BLOCKS_STATUS[200];
	unsigned char ln[6] = { 0,0,44,0,0,255 };
	if ((ret = INIT_OBJECT(imageTransfer, DLMS_OBJECT_TYPE_IMAGE_TRANSFER, ln)) == 0)
	{
		imageTransfer.imageBlockSize = 100;
		imageTransfer.imageFirstNotTransferredBlockNumber = 0;
		/*//Enable image transfer.*/
		imageTransfer.imageTransferEnabled = 1;
		ARR_ATTACH(imageTransfer.imageActivateInfo, IMAGE_ACTIVATE_INFO, 0);
		BIT_ATTACH(imageTransfer.imageTransferredBlocksStatus, IMAGE_TRANSFERRED_BLOCKS_STATUS, 0);
	}
	return ret;
}

void time_now(
gxtime *value, unsigned char meterTime);

/*  Define what is serialized to decrease EEPROM usage. */
gxSerializerIgnore NON_SERIALIZED_OBJECTS[] = {
	/*  Nothing is saved when authentication is not used. */
IGNORE_ATTRIBUTE(BASE(associationPublic), GET_ATTRIBUTE_ALL())};




static int32_t init_security_level(gxAssociationLogicalName* targetassociation,DLMS_AUTHENTICATION level);

static void init_ASS_LOW_SEC_OBJECTS(void)
{
	uint16_t data_Index = 0;
	uint16_t register_Index = 0;
	uint16_t objects_index =0;
	for(objects_index =0;objects_index < max_no_of_data ;objects_index++)
	{
		ASS_LOW_SEC_OBJECTS[objects_index]= BASE(AllData[data_Index]);
		data_Index++;
	}
	
	for(objects_index = max_no_of_data ; objects_index < (max_no_of_data + max_no_of_registers);objects_index++)
	{
		ASS_LOW_SEC_OBJECTS[objects_index]= BASE(AllRegister[register_Index]);
		register_Index++;
	}
	
	/*remember to increment elements' number of ASS_LOW_SEC_OBJECTS when you add new element  */
	
	/* //ASS_LOW_SEC_OBJECTS[objects_index++] = BASE(hdlc);   */
	/* //ASS_LOW_SEC_OBJECTS[objects_index++] = BASE(eventLog); */
	ASS_LOW_SEC_OBJECTS[objects_index++] = BASE(clock1);
	ASS_LOW_SEC_OBJECTS[objects_index++] = BASE(clock_date);
	ASS_LOW_SEC_OBJECTS[objects_index++] = BASE(clock_time);
	ASS_LOW_SEC_OBJECTS[objects_index++] = BASE(sapAssignment);
	ASS_LOW_SEC_OBJECTS[objects_index++] = BASE(disconnectControl);
	ASS_LOW_SEC_OBJECTS[objects_index++] = BASE(associationReadOnly);
	ASS_LOW_SEC_OBJECTS[objects_index++] = BASE(associationReadWriter);
	ASS_LOW_SEC_OBJECTS[objects_index++] = BASE(imageTransfer);
	
}

static int32_t init_obis_association_public_client(void)
{

	/*int ret;*/
	const unsigned char ln[6] = OBIS_LOGICAL_NAME_ASSOCIATION_PUBLIC;
	cosem_init2((gxObject *)&associationPublic, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, ln);
	oa_attach(&associationPublic.objectList, ASS_NO_SEC_OBJECTS, sizeof(ASS_NO_SEC_OBJECTS) / sizeof(ASS_NO_SEC_OBJECTS[0]));
	
	associationPublic.clientSAP = CLIENT_PUBLIC; /* /// Ask */
	associationPublic.serverSAP = SERVER_ADDR;
	
	/* //BB_ATTACH(associationNone.securitySetup->serverSystemTitle, SERVER_SYSTEM_TITLE, 8); */
	/* //BB_ATTACH(associationNone.securitySetup->clientSystemTitle, CLIENT_SYSTEM_TITLE, 8); */
	
	
	return init_security_level(&associationPublic, DLMS_AUTHENTICATION_NONE);
	
}
static int32_t init_obis_association_readonly_account(void)
{

	/*int ret;*/
	const unsigned char ln[6] = OBIS_LOGICAL_NAME_ASSOCIATION_RO;
	cosem_init2((gxObject*)&associationReadOnly, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, ln);
	
	OA_ATTACH(associationReadOnly.objectList, ASS_LOW_SEC_OBJECTS);

	/* //  oa_attach(&associationLow.objectList, ASS_LOW_SEC_OBJECTS, sizeof(ASS_LOW_SEC_OBJECTS) / sizeof(ASS_LOW_SEC_OBJECTS[0])); */
	
	associationReadOnly.clientSAP = CLIENT_RO;
	associationReadOnly.serverSAP = SERVER_ADDR;
		
	return init_security_level(&associationReadOnly, DLMS_AUTHENTICATION_HIGH_ECDSA);
}

static int32_t init_obis_association_readwrite_account(void)
{

	/*int ret;*/
	const unsigned char ln[6] = OBIS_LOGICAL_NAME_ASSOCIATION_RW;
	cosem_init2((gxObject*)&associationReadWriter, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, ln);
	
	OA_ATTACH(associationReadWriter.objectList, ASS_LOW_SEC_OBJECTS);

	/* //  oa_attach(&associationLow.objectList, ASS_LOW_SEC_OBJECTS, sizeof(ASS_LOW_SEC_OBJECTS) / sizeof(ASS_LOW_SEC_OBJECTS[0])); */
	
	associationReadWriter.clientSAP = CLIENT_RW;
	associationReadWriter.serverSAP = SERVER_ADDR;

	/*return init_security_level(&associationReadWriter, DLMS_AUTHENTICATION_HIGH_ECDSA);*/
	return init_security_level(&associationReadWriter, DLMS_AUTHENTICATION_LOW);/*TODO ECSD */
}

static int32_t init_obis_association_fw_account(void)
{

	/*int ret;*/
	const unsigned char ln[6] = OBIS_LOGICAL_NAME_ASSOCIATION_FW;
	cosem_init2((gxObject*)&associationFW, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, ln);
	
	OA_ATTACH(associationFW.objectList, ASS_LOW_SEC_OBJECTS);

	/* //  oa_attach(&associationLow.objectList, ASS_LOW_SEC_OBJECTS, sizeof(ASS_LOW_SEC_OBJECTS) / sizeof(ASS_LOW_SEC_OBJECTS[0])); */
	
	associationFW.clientSAP = CLIENT_FW;
	associationFW.serverSAP = SERVER_ADDR;
	
	return init_security_level(&associationFW, DLMS_AUTHENTICATION_HIGH_ECDSA);
}

static int32_t init_security_level(gxAssociationLogicalName* targetassociation,DLMS_AUTHENTICATION level)
{
	targetassociation->authenticationMechanismName.mechanismId = level;
	switch(level){
		case  DLMS_AUTHENTICATION_NONE:
		targetassociation->securitySetup = NULL;
		break;
		case DLMS_AUTHENTICATION_LOW:{
			
			bb_addString(&targetassociation->secret, "12345678");
			targetassociation->securitySetup = NULL;
		}
		break;
		case DLMS_AUTHENTICATION_HIGH_GMAC:
		case DLMS_AUTHENTICATION_HIGH_ECDSA:
		{
			/* //User list. */
			typedef struct
			{
				/* //User ID. */
				unsigned char id;
				/* //User name */
				char name[MAX_USER_NAME_LENGTH];
			}gxUser;
			static gxUser USER_LIST[10] = { 0 };

			/* //Dedicated key. */

			static unsigned char CYPHERING_INFO[16] = { 0 };
			BB_ATTACH(targetassociation->xDLMSContextInfo.cypheringInfo, CYPHERING_INFO, 0);

			/* //All objects are add for this Association View later. */

			ARR_ATTACH(targetassociation->userList, USER_LIST, 0);
		/* 	//GMAC authentication don't need password. */
		/* 	//Only Authenticated encrypted connections are allowed. */
			BB_ATTACH(targetassociation->securitySetup->serverSystemTitle, SERVER_SYSTEM_TITLE, 8);
			BB_ATTACH(targetassociation->securitySetup->clientSystemTitle, CLIENT_SYSTEM_TITLE, 8);
			targetassociation->securitySetup->securityPolicy = DLMS_SECURITY_POLICY_AUTHENTICATED_ENCRYPTED;
			
			if(level == DLMS_AUTHENTICATION_HIGH_GMAC) {
				targetassociation->securitySetup->securitySuite = DLMS_SECURITY_SUITE_V0;
			} else if(level == DLMS_AUTHENTICATION_HIGH_ECDSA) {
				targetassociation->securitySetup->securitySuite = DLMS_SECURITY_SUITE_V1;
			}
			
			/* //#ifndef DLMS_IGNORE_OBJECT_POINTERS */
			/* // */
			/* //targetassociation->securitySetup = &securitySetupHighGMac; */
			/* // */
			/* //#else */
			/* // */
			/* //memcpy(targetassociation->securitySetupReference, targetassociation.base.logicalName, 6); */
			/* // */
			/* //#endif //DLMS_IGNORE_OBJECT_POINTERS */
		}
		
		break;
	}
	
	return 1;

}
#pragma endregion client setup


static int32_t init_iec_hdlc_setup(dlmsServerSettings *settings)
{

	int ret = 0;
	unsigned char ln[6] = OBIS_LOGICAL_IEC_HDLC;
	cosem_init2((gxObject *)&hdlc, DLMS_OBJECT_TYPE_IEC_HDLC_SETUP, ln);
	if ((ret = INIT_OBJECT(hdlc, DLMS_OBJECT_TYPE_IEC_HDLC_SETUP, ln)) == 0)
	{
		hdlc.communicationSpeed = DLMS_BAUD_RATE_19200;
		hdlc.windowSizeReceive = hdlc.windowSizeTransmit = 1;
		hdlc.maximumInfoLengthTransmit = hdlc.maximumInfoLengthReceive = settings->base.maxInfoRX;
		hdlc.inactivityTimeout = 120;
		hdlc.deviceAddress = 0x01; /* // Ask */
	}
	settings->hdlc = &hdlc;
	return ret;
}

int init_clock(void)
{
	int ret = 0;
	/* // Add default clock. Clock's Logical Name is 0.0.1.0.0.255. */
	const unsigned char ln[6] = OBIS_LOGICAL_CLOCK;
	
	/* //todo Marwan :add date and time obis new date and time objects */
	const unsigned char Dateln[6] = OBIS_LOGICAL_NAME_DATE;
	const unsigned char Timeln[6] = OBIS_LOGICAL_NAME_TIME;

	/* //-------------------------------------------------------------- */
	cosem_init2((gxObject *)&clock1, DLMS_OBJECT_TYPE_CLOCK, ln);
	cosem_init2((gxObject *)&clock_date,DLMS_OBJECT_TYPE_CLOCK,Dateln);
	cosem_init2((gxObject *)&clock_time,DLMS_OBJECT_TYPE_CLOCK,Timeln);

	
	
	{
		#if 0 /* // Todo Sara : Add init code */
		/*//Set default values.*/
		time_init(&clock1.begin, 2023, 5, 3, 3, 0, 0, 0, 0);
		clock1.begin.extraInfo = DLMS_DATE_TIME_EXTRA_INFO_LAST_DAY;
		/*//time_init(&clock1.end, -1, 10, -1, 3, 0, 0, 0, 0);
		//clock1.end.extraInfo = DLMS_DATE_TIME_EXTRA_INFO_LAST_DAY;
		//Meter is using UTC time zone.*/
		clock1.timeZone = 0;
		/*//Deviation is 60 minutes.*/
		clock1.deviation = 60;
		clock1.clockBase = DLMS_CLOCK_BASE_FREQUENCY_50;
		#endif
	}
	return ret;
}

int Init_SapAssignment()
{
	int ret;
	static gxSapItem SAP_ITEMS[5];
	const unsigned char ln[6] = OBIS_SAP_LIST;
	cosem_init2((gxObject *)&sapAssignment, DLMS_OBJECT_TYPE_SAP_ASSIGNMENT, ln);
	{
		SAP_ITEMS[0].name.size = 16;
		SAP_ITEMS[0].id = 0x91;
		SAP_ITEMS[1].name.size = 16;
		SAP_ITEMS[1].id = 0x91;
		/* //ARR_ATTACH(sapAssignment.sapAssignmentList, SAP_ITEMS, 2); */
	}
	return ret;
}



int Init_RelayConnectControl()
{
	int ret;
	const unsigned char ln[6] = OBIS_RELAY_CONNECTION_CONTROL;
	ret = cosem_init2((gxObject *)&disconnectControl, DLMS_OBJECT_TYPE_DISCONNECT_CONTROL, ln);
	/*if (ret == 0)
	{

	}*/
	return ret;
}


int32_t init_obis(dlmsServerSettings *settings)
{

	
	init_all_data(AllData);
	Init_all_register(AllRegister);
	init_ASS_LOW_SEC_OBJECTS();
	init_iec_hdlc_setup(settings);
	init_clock();
	Init_SapAssignment();
	Init_RelayConnectControl();
	addImageTransfer();
	
	init_obis_association_public_client();
	init_obis_association_readwrite_account();
	init_obis_association_readonly_account();
	init_obis_association_fw_account();
	
	OA_ATTACH(settings->base.objects, ASS_LOW_SEC_OBJECTS);
	
	return 0;
}

int saveSecurity(
dlmsSettings *settings)
{
	int ret = 0; 
	return ret;
}

unsigned long getIpAddress()
{
	int ret = -1;
	return ret;
}

int loadSecurity(dlmsSettings *settings)
{ 
	return 0;
}

int svr_findObject(
dlmsSettings *settings,
DLMS_OBJECT_TYPE objectType,
int sn,
unsigned char *ln,
gxValueEventArg *e)
{
	uint16_t pos;
	objectArray objects;
	gxObject *tmp[6];
	oa_attach(&objects, tmp, sizeof(tmp) / sizeof(tmp[0]));
	objects.size = 0;
	if (oa_getObjects(&settings->objects, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, &objects) == 0)
	{
		gxAssociationLogicalName *a;
		for (pos = 0; pos != objects.size; ++pos)
		{
			if (oa_getByIndex(&objects, pos, (gxObject **)&a) == 0)
			{
				if (a->clientSAP == settings->clientAddress &&
				a->authenticationMechanismName.mechanismId == settings->authentication)
				{
					e->target = (gxObject *)a;
					break;
				}
			}
		}
	}
	return 0;
}

/* // Returns current time. */
/* // If you are not using operating system you have to implement this by yourself. */
/* // Reason for this is that all compilers' or HWs don't support time at all. */
void time_now(
gxtime *value, unsigned char meterTime)
{
	/* // time_initUnix(value, (unsigned long)time(NULL)); */
	/* // If date time is wanted in meter time. */
	if (meterTime)
	{
		clock_utcToMeterTime(&clock1, value);
	}
}

void getDateTime_(gxByteBuffer *bb,gxClock *ctime)
{
	ctime->time.value.tm_year=(bb->data[0]<<8 |bb->data[1]);
	ctime->time.value.tm_mon=(bb->data[2]);
	ctime->time.value.tm_mday=(bb->data[3]);
	ctime->time.value.tm_hour=(bb->data[5]);
	ctime->time.value.tm_min=(bb->data[6]);
	ctime->time.value.tm_sec=(bb->data[7]);
}
void getDate_(gxByteBuffer *bb , gxClock *cdate){
	cdate->time.value.tm_year=(bb->data[0]<<8 |bb->data[1]);
	cdate->time.value.tm_mon=(bb->data[2]);
	cdate->time.value.tm_mday=(bb->data[3]);

}

void gettime_(gxByteBuffer *bb , gxClock *ctime){
	ctime->time.value.tm_hour=(bb->data[0]);
	ctime->time.value.tm_min=(bb->data[1]);
	ctime->time.value.tm_sec=(bb->data[2]);
}
int Obj_HandleClockSetAttr(gxByteBuffer *bb, char index)
{


	int ret = 0;
	switch (index)
	{
		case 2: 
		 getDateTime_(bb,&clock1);
		rtc_set_data_time(&clock1.time.value.tm_year,&clock1.time.value.tm_mon,&clock1.time.value.tm_mday,&clock1.time.value.tm_wday,
				&clock1.time.value.tm_hour,&clock1.time.value.tm_min,&clock1.time.value.tm_sec);
 		 break;
		case 3:
		break;
		case 4:
		break;
		case 5:
		break;
		case 6:
		break;
		case 7:
		break;
		case 8:
		break;
		case 9:
		break;
		default:
		break;
	}
	return ret;
}

int Obj_HandleClockGetAttr(gxByteBuffer *bb, char index)
{
	int ret = 0;
	gxtime dt; /* // toDo unused var just use it cause of Cosem Function to be removed in future */
	dt.value.tm_wday=0;
	/*uint32_t x=0;*/
	
	
	switch (index)
	{
		case 2:		/*Attribute : time */
	rtc_get_data_time(&dt.value.tm_year,&dt.value.tm_mon,&dt.value.tm_mday,&dt.value.tm_wday,
						&dt.value.tm_hour,&dt.value.tm_min,&dt.value.tm_sec);

			
		ret = cosem_setDateTimeAsOctetString(bb, &dt);
		break;
		case 3:/*Attribute : time_zone */
		break;
		case 4:/*Attribute : status */
		dt.status =DLMS_CLOCK_STATUS_OK;
		break;
		case 5:/*Attribute : daylight_savings_begin */
		
		break;
		case 6:/*Attribute : daylight_savings_end */
		
		break;
		case 7:/*Attribute : daylight_savings_deviation */
		
		break;
		case 8:/*Attribute : daylight_savings_enabled */
		
		break;
		case 9:/*Attribute : clock_base */
		
		break;
		default:
		break;
	}
	
	
	return ret;
}

int Obj_HandleClockActMethod(dlmsVARIANT parameters, char index)
{
	/*int ret = 0;*/
	switch (index)
	{
		case 1: /* // adjust_to_quarter (integer data)
		// Todo Add Requested Action */
		
		break;
		case 2: /* // adjust_to_measuring_period (integer data) */
		
		break;
		case 3: /* // adjust_to_minute (integer data) */
		
		break;
		case 4: /* // adjust_to_preset_time (integer data) */
		
		break;
		case 5:/*  // preset_adjusting_time (structure data) */
		
			/*data ::= structure
			{
			preset_time: octet-string,
			validity_interval_start: octet-string,
			validity_interval_end: octet-string
			}*/
		
		break;
		case 6: /* // shift_time (long data) */
		break;
		default:
		break;
	}
}

int Obj_HandleRelayActMethod(char index)
{
	/*int ret = 0;*/
	extern config_t dataConfig;
	switch (index)
	{
		/* Disconnect Mode*/
		
		case 1: /* // adjust_to_quarter (integer data)
		// Todo Add Requested Action */	
		set_event(EVENT_NAME_REMOTE_CONTROL);
		if (dataConfig.bits.DisconnectForce == 1)
		{
			set_event(EVENT_NAME_FORCE_DISCONNECT);
			remove_event(EVENT_NAME_FORCE_CONNECT);
		}		
		break;
		
		/* Connect Mode*/
		case 2: /* // adjust_to_measuring_period (integer data) */

		remove_event(EVENT_NAME_REMOTE_CONTROL);
		if (dataConfig.bits.ConnectForce == 1)
		{
			set_event(EVENT_NAME_FORCE_CONNECT);
			remove_event(EVENT_NAME_FORCE_DISCONNECT);
		}
		break;
		default:
		break;
	}
}
/* //todo Marwan : add date and time obis new date and time objects */
int Obj_HandleTimeSetAttr(gxByteBuffer *bb, char index)
{
	int ret = 0;
	switch (index)
	{
		case 2:
		
		gettime_(bb,&clock_time);
		rtc_set_time(&clock_time.time.value.tm_hour, &clock_time.time.value.tm_min , &clock_time.time.value.tm_sec);
		break;
		case 3:
		break;
		case 4:
		break;
		case 5:
		break;
		case 6:
		break;
		case 7:
		break;
		case 8:
		break;
		case 9:
		break;
		default:
		break;
	}
	return ret;
}

int Obj_HandleTimeGetAttr(gxByteBuffer *bb, char index)
{
	int ret = 0;
	gxtime dt;/*  // toDo unused var just use it cause of Cosem Function to be removed in future */
	dt.value.tm_wday=0xFF;
	dt.value.tm_year=0xFF;
	dt.value.tm_mon=0xFF;
	dt.value.tm_mday=0xFF;
	
	/*uint32_t x=0;*/
	
	switch (index)
	{
		case 2:		/*Attribute : time */
		 rtc_get_time(&dt.value.tm_hour,&dt.value.tm_min,&dt.value.tm_sec);
		ret = cosem_setDateTimeAsOctetString(bb,&dt);
		break;
		case 3:/*Attribute : time_zone */
		
		break;
		case 4:/*Attribute : status */
		dt.status =DLMS_CLOCK_STATUS_OK;
		break;
		case 5:/*Attribute : daylight_savings_begin */
		
		break;
		case 6:/*Attribute : daylight_savings_end */
		
		break;
		case 7:/*Attribute : daylight_savings_deviation */
		
		break;
		case 8:/*Attribute : daylight_savings_enabled */
		
		break;
		case 9:/*Attribute : clock_base */
		
		break;
		default:
		break;
	}
	
	
	return ret;
}

int Obj_HandleDateSetAttr(gxByteBuffer *bb, char index)
{
	int ret = 0;
	switch (index)
	{
		case 2:

		getDate_(bb,&clock_date);
		
		rtc_set_date (&clock_date.time.value.tm_year , &clock_date.time.value.tm_mon, &clock_date.time.value.tm_mday ,&clock_date.time.value.tm_wday );
		break;
		case 3:
		break;
		case 4:
		break;
		case 5:
		break;
		case 6:
		break;
		case 7:
		break;
		case 8:
		break;
		case 9:
		break;
		default:
		break;
	}
	return ret;
}

int Obj_HandleDateGetAttr(gxByteBuffer *bb, char index)
{
	int ret = 0;
	gxtime dt;/*  // toDo unused var just use it cause of Cosem Function to be removed in future */
	dt.value.tm_wday=0xFF;
	dt.value.tm_hour=0xFF;
	dt.value.tm_min=0xFF;
	dt.value.tm_sec=0xFF;
	/*uint32_t x=0;*/
	 
	switch (index)
	{
		case 2:		/*Attribute : time */

		rtc_get_date(&dt.value.tm_year,&dt.value.tm_mon,&dt.value.tm_mday,&dt.value.tm_wday);

		ret = cosem_setDateTimeAsOctetString(bb, &dt);
		break;
		case 3:/*Attribute : time_zone */
		
		break;
		case 4:/*Attribute : status */
		dt.status =DLMS_CLOCK_STATUS_OK;
		break;
		case 5:/*Attribute : daylight_savings_begin */
		
		break;
		case 6:/*Attribute : daylight_savings_end */
		
		break;
		case 7:/*Attribute : daylight_savings_deviation */
		
		break;
		case 8:/*Attribute : daylight_savings_enabled */
		
		break;
		case 9:/*Attribute : clock_base */
		
		break;
		default:
		break;
	}
	
	
	return ret;
}


/*//--------------------------------------------------------------------*/
void svr_preRead(
dlmsSettings *settings,
gxValueEventCollection *args)
{
	gxValueEventArg *e;
	int ret, pos;
	DLMS_OBJECT_TYPE type;
	for (pos = 0; pos != args->size; ++pos)
	{
		if ((ret = vec_getByIndex(args, pos, &e)) != 0)
		{
			return;
		}
		/* // GXTRACE_LN(GET_STR_FROM_EEPROM("svr_preRead: "), e->target->objectType, e->target->logicalName);
		// Let framework handle Logical Name read. */
		if (e->index == 1)
		{
			continue;
		}

	/* 	// Get target type. */
		type = (DLMS_OBJECT_TYPE)e->target->objectType;
		/* // Let Framework will handle Association objects and profile generic automatically. */
		if (type == DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME ||
		type == DLMS_OBJECT_TYPE_ASSOCIATION_SHORT_NAME)
		{
			continue;
		}
		/* // generic handle for register object */
		else if (type == DLMS_OBJECT_TYPE_REGISTER)
		{
			e->handled = 1;
			e->error =handle_register_read_callback( AllRegister, e->target->logicalName,e->index,e->value.byteArr,&e->value.vt);
			continue;
		}/* // generic handle for register object */
		else if (type == DLMS_OBJECT_TYPE_DATA)
		{
			
			e->error =handle_data_read_callback( AllData, e->target->logicalName,e->index,e->value.byteArr,&e->value.vt);
			if(e->value.vt == DLMS_DATA_TYPE_ARRAY || e->value.vt == DLMS_DATA_TYPE_STRUCTURE || DLMS_DATA_TYPE_STRING || DLMS_DATA_TYPE_BIT_STRING)
			{
				e->handled = 0;
			}else{
				e->handled = 1;
			}
			continue;
		}
		/* // Get time if user want to read date and time. */
		else if (e->target == BASE(clock1))
		{
			e->error = Obj_HandleClockGetAttr(e->value.byteArr, e->index);
			e->value.vt = DLMS_DATA_TYPE_DATETIME;
			e->handled = 1;
		}
		else if(e->target == BASE(clock_date)){
			e->error = Obj_HandleDateGetAttr(e->value.byteArr, e->index);
			e->value.vt = DLMS_DATA_TYPE_DATETIME;
			e->handled = 1;		
		}
		
		else if(e->target == BASE(clock_time)){
			e->error = Obj_HandleTimeGetAttr(e->value.byteArr, e->index);
			e->value.vt = DLMS_DATA_TYPE_DATETIME;
			e->handled = 1;
		}
		else if (e->target == BASE(imageTransfer))
		{
			
			image_transfer_preRead_callback(e,&imageTransfer);
		}
		/* // todo Add ret of obj  or make array of pointer to functions of all objects depending on their position _Note tricky */
		else
		{
		}
	}
}
void svr_preWrite(
dlmsSettings *settings,
gxValueEventCollection *args)
{
	gxValueEventArg *e;
	int ret, pos;
	DLMS_OBJECT_TYPE type;
	for (pos = 0; pos != args->size; ++pos)
	{
		if ((ret = vec_getByIndex(args, pos, &e)) != 0)
		{
			return;
		}
		if (e->index == 1)
		{
			continue;
		}
		/* // Get target type. */
		type = (DLMS_OBJECT_TYPE)e->target->objectType;
		/* // Update value by one every time when user reads register. */
		if (type == DLMS_OBJECT_TYPE_REGISTER)
		{
			/* //todo asamir handle write like the data write
			//e->handled = 1;
			//handle_register_read_callback( AllRegister, e->target->logicalName,e->index,e->value.byteArr,&e->value.vt); */
			continue;
		}/* // generic handle for register object */
		else if (type == DLMS_OBJECT_TYPE_DATA)
		{
			e->handled = 1;
			e->error = handle_data_write_callback( AllData, e->target->logicalName,e->index,&e->value);
			continue;
		}
		/* // Get time if user want to read date and time. */
		else if (e->target == BASE(clock1))
		{
			e->error = Obj_HandleClockSetAttr(e->value.byteArr, e->index);
			/* // e->value.vt = DLMS_DATA_TYPE_DATETIME; */
			e->handled = 1;
		}
		else if(e->target == BASE(clock_date)){
			e->error = Obj_HandleClockSetAttr(e->value.byteArr, e->index);
			/* // e->value.vt = DLMS_DATA_TYPE_DATETIME; */
			e->handled = 1;
			
		}
		
		else if(e->target == BASE(clock_time)){
			e->error = Obj_HandleClockSetAttr(e->value.byteArr, e->index);
			/* // e->value.vt = DLMS_DATA_TYPE_DATETIME; */
			e->handled = 1;
		}
		else if (e->target == BASE(imageTransfer))
		{
			/* //image_transfer_callback(e,&imageTransfer,IMAGE_ACTIVATE_INFO); */
			image_transfer_preWrite_callback(e,&imageTransfer);
			
		}
	}
}

void svr_preAction(
dlmsSettings *settings,
gxValueEventCollection *args)
{
	gxValueEventArg *e;
	int ret, pos;
	for (pos = 0; pos != args->size; ++pos)
	{
		if ((ret = vec_getByIndex(args, pos, &e)) != 0)
		{
			return;
		}
		if (e->target == BASE(clock1))
		{
			Obj_HandleClockActMethod(e->parameters, e->index);
			e->handled = 1;
		}
		else if (e->target == BASE(disconnectControl))
		{
			Obj_HandleRelayActMethod(e->index);
			e->handled = 1;
		}
		else if (e->target == BASE(imageTransfer))
		{
			image_transfer_preAction_callback(e,&imageTransfer);
		}
	}
}

/* // sara todo chek what is needed */
void svr_postRead(
dlmsSettings *settings,
gxValueEventCollection *args)
{
}
/* // sara todo chek what is needed */
void svr_postWrite(
dlmsSettings *settings,
gxValueEventCollection *args)
{
}
/* // sara todo chek what is needed */
void svr_postAction(
dlmsSettings *settings,
gxValueEventCollection *args)
{
}

unsigned char svr_isTarget(
dlmsSettings *settings,
unsigned long serverAddress,
unsigned long clientAddress)
{
	objectArray objects;
	oa_init(&objects);
	unsigned char ret = 0;
	uint16_t pos;
	gxObject *tmp[6];
	oa_attach(&objects, tmp, sizeof(tmp) / sizeof(tmp[0]));
	objects.size = 0;
	if (oa_getObjects(&settings->objects, DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME, &objects) == 0)
	{
		gxAssociationLogicalName *a;
		for (pos = 0; pos != objects.size; ++pos)
		{
			if (oa_getByIndex(&objects, pos, (gxObject **)&a) == 0)
			{
				if ((a->clientSAP == clientAddress) && (a->serverSAP == serverAddress))
				{
					ret = 1;
					switch (a->authenticationMechanismName.mechanismId)
					{
						case DLMS_AUTHENTICATION_NONE:
						/* // Client connects without authentication. */
						OA_ATTACH(settings->objects, ASS_NO_SEC_OBJECTS);
						break;
						case DLMS_AUTHENTICATION_LOW:
						/* // Client connects using low authentication. */
						break;
						default:
						/* // Client connects using High authentication. */
						{
							OA_ATTACH(settings->objects, ASS_LOW_SEC_OBJECTS);
							int len = 0;
							
							bb_empty(&settings->cipher.authenticationKey);
							unsigned char* a = GET_OCTET_AUTHENTICATION_KEY(&len);
							bb_set(&settings->cipher.authenticationKey,a,len);
							
							
							bb_empty(&settings->cipher.blockCipherKey);
							unsigned char* b = GET_OCTET_BLOCK_CIPHER_KEY(&len);
							bb_set(&settings->cipher.blockCipherKey,b,len);
							
							bb_empty(&settings->kek);
							unsigned char* k = GET_OCTET_KEY_OF_KEYS(&len);
							bb_set(&settings->kek,k,len);
							
							bb_empty(&settings->cipher.systemTitle);
							bb_set(&settings->cipher.systemTitle,SERVER_SYSTEM_TITLE,8);
						}
						break;
					}
					break;
				}
			}
		}
	}
	return ret;
}

DLMS_SOURCE_DIAGNOSTIC svr_validateAuthentication(
dlmsServerSettings *settings,
DLMS_AUTHENTICATION authentication,
gxByteBuffer *password)
{
	if (authentication == DLMS_AUTHENTICATION_NONE)
	{
		/* // Uncomment this if authentication is always required.
		// return DLMS_SOURCE_DIAGNOSTIC_AUTHENTICATION_MECHANISM_NAME_REQUIRED; */
		return DLMS_SOURCE_DIAGNOSTIC_NONE;
	}
	/* // Check Low Level security.. */
	else if (authentication == DLMS_AUTHENTICATION_LOW)
	{
		
		if (bb_compare(password, associationReadWriter.secret.data, associationReadWriter.secret.size) == 0)
		{
			set_event(EVENT_NAME_SEC_AUTH_LOW_FAIL);
			remove_event(EVENT_NAME_SEC_AUTH_LOW_FAIL);
			return DLMS_SOURCE_DIAGNOSTIC_AUTHENTICATION_FAILURE;
		}
	}
	/* // Hith authentication levels are check on phase two. */
	return DLMS_SOURCE_DIAGNOSTIC_NONE;
}

/* // Get attribute access level for profile generic. */
DLMS_ACCESS_MODE getProfileGenericAttributeAccess(
dlmsSettings *settings,
gxObject *obj,
unsigned char index)
{
/* 	// Only read is allowed for event log. */
	if (obj == BASE(eventLog))
	{
		return DLMS_ACCESS_MODE_READ;
	}
	/* // Write is allowed only for High authentication. */
	if (settings->authentication > DLMS_AUTHENTICATION_LOW)
	{
		switch (index)
		{
			case 3: /* // captureObjects. */
			return DLMS_ACCESS_MODE_READ_WRITE;
			case 4: /* // capturePeriod */
			return DLMS_ACCESS_MODE_READ_WRITE;
			case 8:/*  // Profile entries. */
			return DLMS_ACCESS_MODE_READ_WRITE;
			default:
			break;
		}
	}
	return DLMS_ACCESS_MODE_READ;
}

/* // Get attribute access level for Push Setup. */
DLMS_ACCESS_MODE getPushSetupAttributeAccess(
dlmsSettings *settings,
unsigned char index)
{
	/* // Write is allowed only for High authentication. */
	if (settings->authentication > DLMS_AUTHENTICATION_LOW)
	{
		switch (index)
		{
			case 2: /* // pushObjectList */
			case 4: /* // communicationWindow */
			return DLMS_ACCESS_MODE_READ_WRITE;
			default:
			break;
		}
	}
	return DLMS_ACCESS_MODE_READ;
}

/* // Get attribute access level for Disconnect Control. */
DLMS_ACCESS_MODE getDisconnectControlAttributeAccess(
dlmsSettings *settings,
unsigned char index)
{
	return DLMS_ACCESS_MODE_READ;
}

/* // Get attribute access level for register schedule. */
DLMS_ACCESS_MODE getActionSchduleAttributeAccess(
dlmsSettings *settings,
unsigned char index)
{
	/* // Write is allowed only for High authentication. */
	if (settings->authentication > DLMS_AUTHENTICATION_LOW)
	{
		switch (index)
		{
			case 4: /* // Execution time. */
			return DLMS_ACCESS_MODE_READ_WRITE;
			default:
			break;
		}
	}
	return DLMS_ACCESS_MODE_READ;
}

/* // Get attribute access level for register. */
DLMS_ACCESS_MODE getRegisterAttributeAccess(
dlmsSettings *settings,
unsigned char index)
{
	return DLMS_ACCESS_MODE_READ;
}

/* // Get attribute access level for data objects. */
DLMS_ACCESS_MODE getDataAttributeAccess(
dlmsSettings *settings,
unsigned char index)
{
	return DLMS_ACCESS_MODE_READ;
}

/* // Get attribute access level for script table. */
DLMS_ACCESS_MODE getScriptTableAttributeAccess(
dlmsSettings *settings,
unsigned char index)
{
	return DLMS_ACCESS_MODE_READ;
}

/* // Get attribute access level for IEC HDLS setup. */
DLMS_ACCESS_MODE getHdlcSetupAttributeAccess(
dlmsSettings *settings,
unsigned char index)
{
	/* // Write is allowed only for High authentication. */
	if (settings->authentication > DLMS_AUTHENTICATION_LOW)
	{
		switch (index)
		{
			case 2:/*  // Communication speed. */
			case 7:
			case 8:
			return DLMS_ACCESS_MODE_READ_WRITE;
			default:
			break;
		}
	}
	return DLMS_ACCESS_MODE_READ;
}

/* // Get attribute access level for association LN. */
DLMS_ACCESS_MODE getAssociationAttributeAccess(
dlmsSettings *settings,
unsigned char index)
{
	/* // If secret */
	if (settings->authentication == DLMS_AUTHENTICATION_LOW && index == 7)
	{
		return DLMS_ACCESS_MODE_READ_WRITE;
	}
	return DLMS_ACCESS_MODE_READ;
}

/* // Get attribute access level for security setup. */
DLMS_ACCESS_MODE getSecuritySetupAttributeAccess(
dlmsSettings *settings,
unsigned char index)
{
	/* // Only client system title is writable. */
	if (settings->authentication > DLMS_AUTHENTICATION_LOW && index == 4)
	{
		return DLMS_ACCESS_MODE_READ_WRITE;
	}
	return DLMS_ACCESS_MODE_READ;
}

/* // Get attribute access level for security setup. */
DLMS_ACCESS_MODE getActivityCalendarAttributeAccess(
dlmsSettings *settings,
unsigned char index)
{
	/* // Only Activate passive calendar date-time and passive calendar settings are writeble. */
	if (settings->authentication > DLMS_AUTHENTICATION_LOW && index > 5)
	{
		return DLMS_ACCESS_MODE_READ_WRITE;
	}
	return DLMS_ACCESS_MODE_READ;
}

/**
* Get attribute access level.
*/
DLMS_ACCESS_MODE svr_getAttributeAccess(
dlmsSettings *settings,
gxObject *obj,
unsigned char index)
{
	
	/* // Only read is allowed if authentication is not used. */
	if (index == 1 || settings->authentication == DLMS_AUTHENTICATION_NONE)
	{
		return DLMS_ACCESS_MODE_READ;
	}
	if (obj->objectType == DLMS_OBJECT_TYPE_ASSOCIATION_LOGICAL_NAME)
	{
		return getAssociationAttributeAccess(settings, index);
	}
	if (obj->objectType == DLMS_OBJECT_TYPE_PROFILE_GENERIC)
	{
		return getProfileGenericAttributeAccess(settings, obj, index);
	}
	if (obj->objectType == DLMS_OBJECT_TYPE_PUSH_SETUP)
	{
		return getPushSetupAttributeAccess(settings, index);
	}
	if (obj->objectType == DLMS_OBJECT_TYPE_DISCONNECT_CONTROL)
	{
		return getDisconnectControlAttributeAccess(settings, index);
	}
	if (obj->objectType == DLMS_OBJECT_TYPE_DISCONNECT_CONTROL)
	{
		return getDisconnectControlAttributeAccess(settings, index);
	}
	if (obj->objectType == DLMS_OBJECT_TYPE_ACTION_SCHEDULE)
	{
		return getActionSchduleAttributeAccess(settings, index);
	}
	if (obj->objectType == DLMS_OBJECT_TYPE_SCRIPT_TABLE)
	{
		return getScriptTableAttributeAccess(settings, index);
	}
	if (obj->objectType == DLMS_OBJECT_TYPE_REGISTER)
	{
		return getRegisterAttributeAccess(settings, index);
	}
	if (obj->objectType == DLMS_OBJECT_TYPE_DATA)
	{
		switch (settings->clientAddress)
		{
			case CLIENT_PUBLIC :
			return obj->access->attributeAccessModes.data[0];
			break;
			case CLIENT_RO :
			return obj->access->attributeAccessModes.data[1];
			break;
			case CLIENT_RW :
			return obj->access->attributeAccessModes.data[2];
			break;
			case CLIENT_FW :
			return obj->access->attributeAccessModes.data[3];
			break;
		}
		/* //return getDataAttributeAccess(settings, index); */
	}
	if (obj->objectType == DLMS_OBJECT_TYPE_IEC_HDLC_SETUP)
	{
		return getHdlcSetupAttributeAccess(settings, index);
	}
	if (obj->objectType == DLMS_OBJECT_TYPE_SECURITY_SETUP)
	{
		return getSecuritySetupAttributeAccess(settings, index);
	}
	if (obj->objectType == DLMS_OBJECT_TYPE_ACTIVITY_CALENDAR)
	{
		return getActivityCalendarAttributeAccess(settings, index);
	}
	/* // Only clock write is allowed. */
	if (settings->authentication == DLMS_AUTHENTICATION_LOW)
	{
		if (obj->objectType == DLMS_OBJECT_TYPE_CLOCK)
		{
			return DLMS_ACCESS_MODE_READ_WRITE;
		}
		return DLMS_ACCESS_MODE_READ;
	}
	/* // All writes are allowed. */
	return DLMS_ACCESS_MODE_READ_WRITE;
}

/**
* Get method access level.
*/
DLMS_METHOD_ACCESS_MODE svr_getMethodAccess(
dlmsSettings *settings,
gxObject *obj,
unsigned char index)
{
	/* // Methods are not allowed. */
	if (settings->authentication == DLMS_AUTHENTICATION_NONE)
	{
		return DLMS_METHOD_ACCESS_MODE_NONE;
	}
	/* // Only clock methods are allowed. */
	if (settings->authentication == DLMS_AUTHENTICATION_LOW)
	{
		if ((obj->objectType == DLMS_OBJECT_TYPE_CLOCK)||(obj->objectType == DLMS_OBJECT_TYPE_DISCONNECT_CONTROL))
		{
			return DLMS_METHOD_ACCESS_MODE_ACCESS;
		}else
		if(obj->objectType == DLMS_OBJECT_TYPE_IMAGE_TRANSFER){
			return DLMS_ACCESS_MODE_READ_WRITE;
		}
		return DLMS_METHOD_ACCESS_MODE_NONE;
	}
	return DLMS_METHOD_ACCESS_MODE_ACCESS;
}

int svr_connected(
dlmsServerSettings *settings)
{
	loadSecurity(&settings->base);
	return 0;
}

/**
* Client has try to made invalid connection. Password is incorrect.
*
* @param connectionInfo
*            Connection information.
*/
int svr_invalidConnection(dlmsServerSettings *settings)
{
	return 0;
}

int svr_disconnected(
dlmsServerSettings *settings)
{
	if (settings->base.cipher.security != 0 && (settings->base.connected & DLMS_CONNECTION_STATE_DLMS) != 0)
	{
		/* // Save Invocation counter value when connection is closed. */
		saveSecurity(&settings->base);
	}
	return 0;
}

void svr_preGet(
dlmsSettings *settings,
gxValueEventCollection *args)
{
	gxValueEventArg *e;
	int ret, pos;
	for (pos = 0; pos != args->size; ++pos)
	{
		if ((ret = vec_getByIndex(args, pos, &e)) != 0)
		{
			return;
		}
	}
}

void svr_postGet(
dlmsSettings *settings,
gxValueEventCollection *args)
{
}


