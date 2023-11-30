#include "eeprom.h"
#include "e2prom/eeprom_lookup.h"
#include "task.h"
#include "utilities.h"
#include "gpio.h"
#include "Delay/delay.h"
#include "i2c.h"


#ifdef __cplusplus
extern "C"
{
#endif

#define E2prom_number_bak 3
#define EEPROM_DEBUG

extern I2C_HandleTypeDef hi2c2;

#define EEPROM_I2C &hi2c2
#define EEPROM_ADDR 0xA0
#define PAGE_SIZE 64

    ErrFlag_Struct VErrFlag;
    u8 VWrE2SafeByte;

    /*
    //=========================================================
    //description	::	EEprom bus Initation
    //function		::	BSP_E2promBus_Init
    //Input			::	none
    //Output		::	none
    //call			::	I2C_START,I2C_STOP
    //effect		::	none
    //=========================================================
    */
    void BSP_E2promBus_Init(void)
    {
        /*TODO :: I2C Initialization*/
        /*     EEPROMOpen();

            I2C_SDA_Out();
            I2C_SCL_Out();

            I2C_START();
            __NOP();
            __NOP();
            I2C_STOP();
            __NOP(); */
    }
    uint16_t bytestowrite(uint16_t size, uint16_t offset)
    {
        if ((size + offset) < PAGE_SIZE)
            return size;
        else
            return PAGE_SIZE - offset;
    }

    /**
     * @brief
     *
     * @param address
     * @return uint16_t
     */
    u16 calculate_page_number(u32 address)
    {

        return address / PAGE_SIZE;
    }
    /*
    //=========================================================
    //decription	::	read size byte data from e2prom by e2 bus
    //function		::	EEPROMreader
    //Input			::	offset->read E2 address
    //					buffer->data ram pointer
    //					size->read data len
    //Output		::	read out data len
    //call			::	I2C.aperator
    //effect		::	if VErrFlag.BIT.RdE2ErrFlag=1 then e2 is error
    //=========================================================
    */
    u16 EEPROMreader(u32 address, u8 *buffer, u16 size)
    {

        /* Find out the number of bit, where the page addressing starts */
        int paddrposition = log(PAGE_SIZE) / log(2);
        /* Find out the offset from the base address of the page */
        uint16_t offset = address % PAGE_SIZE;
        /* Calculate the start page of the passed address  */
        uint16_t startPage = calculate_page_number(address);
        /* Find the end page */
        uint16_t endPage = startPage + ((size + offset) / PAGE_SIZE);
        /* Find number of pages to be written */
        uint16_t numofpages = (endPage - startPage) + 1;
        uint16_t pos = 0;

        for (u32 i = 0; i < numofpages; i++)
        {
            uint8_t retVal = 1;
            /* calculate the address of the memory location
             * Here we add the page address with the byte address
             */
            uint16_t MemAddress = startPage << paddrposition | offset;
            uint16_t bytesremaining = bytestowrite(size, offset); /*// calculate the remaining bytes to be written*/

            retVal = HAL_I2C_Mem_Read(EEPROM_I2C, EEPROM_ADDR, MemAddress, 2, &buffer[pos], bytesremaining, 1000); /*// write the data to the EEPROM*/
            if (retVal == 0)
            {
                VErrFlag.BIT.RdE2ErrFlag = 0;
            }
            else
            {
                VErrFlag.BIT.RdE2ErrFlag = 1;
                size = 0;
                break;
            }
            startPage += 1;               /*  increment the page, so that a new page address can be selected for further write */
            offset = 0;                   /*  since we will be writing to a new page, so offset will be 0 */
            size = size - bytesremaining; /*  reduce the size of the bytes */
            pos += bytesremaining;        /*  update the position for the data buffer */
            HAL_Delay(5);
        }

#if 0
/* SW I2C Implementation */
        u8 WrDeviceAddr;
        u16 RdLen;
        u8 RdE2PgInAdr, RdE2PgAdr;
        u8 count, len;
        u8 ptroffset = 0;
        WrDeviceAddr = WrE2_DeviceAddr;
        if (offset >> 16)
        {
            WrDeviceAddr |= 0x02; /*//high 16 bits address visitor*/
        }
        RdE2PgInAdr = (u8)(offset);
        RdE2PgAdr = (u8)(offset >> 8);
        RdLen = 0;
        while (size > 0)
        {
            VErrFlag.BIT.RdE2ErrFlag = 1;

            I2C_START();
            Write_I2C_1Byte(WrDeviceAddr); /*//write device address*/
            if (VErrFlag.BIT.IIcNoAckFlag)
            {
                break;
            }
            Write_I2C_1Byte(RdE2PgAdr); /*//page in high address*/
            if (VErrFlag.BIT.IIcNoAckFlag)
            {
                break;
            }
            Write_I2C_1Byte(RdE2PgInAdr); /*//page in low address*/
            if (VErrFlag.BIT.IIcNoAckFlag)
            {
                break;
            }

            I2C_START();
            Write_I2C_1Byte(WrDeviceAddr | 0x01);
            if (VErrFlag.BIT.IIcNoAckFlag)
            {
                break;
            }

            if (((RdE2PgInAdr % 0x40) + size) > 0x40)
            {
                len = ((RdE2PgInAdr / 0x40) + 1) * 0x40 - RdE2PgInAdr;
            } // it is not finished
            else
            {
                len = size;
            } // it is finished
            for (count = 0; count < len - 1; count++)
            {
                *(buffer + count + ptroffset) = Read_I2C_1ByteAck();
            }
            *(buffer + count + ptroffset) = Read_I2C_1ByteNoAck();
            VErrFlag.BIT.RdE2ErrFlag = 0;

            I2C_STOP();
            ptroffset = ptroffset + len;
            size = size - len;
            RdLen = RdLen - len;
            if ((u8)(RdE2PgInAdr + len) < RdE2PgInAdr)
            {
                RdE2PgAdr++;
                if (RdE2PgAdr == 0)
                {
                    WrDeviceAddr |= 0x02; /*//high 16 bits address visitor*/
                }
            }
            RdE2PgInAdr = RdE2PgInAdr + len;
        }
        if (VErrFlag.BIT.RdE2ErrFlag)
        {
            I2C_STOP();
        }
#endif

        return (size);
    }

    /*
    //=========================================================
    //decription	::	write n byte data to e2prom by e2 bus
    //function		::	WriteE2promData
    //Input			::	offset->write E2 address
    //					buffer->data ram pointer
    //					size->write data len
    //Output		::	write to data len
    //call			::	I2C.aperator
    //effect		::	if VErrFlag.BIT.WrE2ErrFlag=1 then e2 is error
    //=========================================================
    */
    u16 EEPROMwriter(u32 address, u8 *buffer, u16 size)
    {

        /* Find out the number of bit, where the page addressing starts */
        int paddrposition = log(PAGE_SIZE) / log(2);
        /* Find out the offset from the base address of the page */
        uint16_t offset = address % PAGE_SIZE;
        /* Calculate the start page of the passed address  */
        uint16_t startPage = calculate_page_number(address);
        /* Find the end page */
        uint16_t endPage = startPage + ((size + offset) / PAGE_SIZE);
        /* Find number of pages to be written */
        uint16_t numofpages = (endPage - startPage) + 1;
        uint16_t pos = 0;

        for (u32 i = 0; i < numofpages; i++)
        {
            uint8_t retVal = 1;
            /* calculate the address of the memory location
             * Here we add the page address with the byte address
             */
            uint16_t MemAddress = startPage << paddrposition | offset;
            uint16_t bytesremaining = bytestowrite(size, offset); // calculate the remaining bytes to be written

            retVal = HAL_I2C_Mem_Write(EEPROM_I2C, EEPROM_ADDR, MemAddress, 2, &buffer[pos], bytesremaining, 1000); // write the data to the EEPROM

            if (retVal == 0)
            {
                VErrFlag.BIT.RdE2ErrFlag = 0;
            }
            else
            {
                VErrFlag.BIT.RdE2ErrFlag = 1;
                size = 0;
                break;
            }
            startPage += 1;               /*  increment the page, so that a new page address can be selected for further write */
            offset = 0;                   /*  since we will be writing to a new page, so offset will be 0 */
            size = size - bytesremaining; /*  reduce the size of the bytes */
            pos += bytesremaining;        /*  update the position for the data buffer */
            HAL_Delay(5);
        }

#if 0
    /*SW I2C Implementation */
        u8 WrDeviceAddr;
        u16 WrLen;
        u8 WrE2PageAddr, WrE2PageInAddr;
        u8 count, len;
        u8 ptroffset = 0;
        WrLen = 0;
        WrDeviceAddr = WrE2_DeviceAddr;
        if (offset >> 16)
        {
            WrDeviceAddr |= 0x02; /*//high 16 bits address visitor*/
        }
        WrE2PageAddr = (u8)(offset >> 8);
        WrE2PageInAddr = (u8)(offset);

        while (size > 0)
        {
            VErrFlag.BIT.WrE2ErrFlag = 1;
            I2C_START();
            Write_I2C_1Byte(WrDeviceAddr); /*//device address*/
            if (VErrFlag.BIT.IIcNoAckFlag)
            {
                break;
            }
            Write_I2C_1Byte(WrE2PageAddr); /*//page in high address*/
            if (VErrFlag.BIT.IIcNoAckFlag)
            {
                break;
            }
            Write_I2C_1Byte(WrE2PageInAddr); /*//page in low address*/
            if (VErrFlag.BIT.IIcNoAckFlag)
            {
                break;
            }
            if (((WrE2PageInAddr % 0x40) + size) > 0x40)
            {
                len = ((WrE2PageInAddr / 0x40) + 1) * 0x40 - WrE2PageInAddr;
            } /*//it is not finished*/
            else
            {
                len = size;
            } /*//it is finished*/
            for (count = 0; count < len; count++)
            {
                Write_I2C_1Byte(*(buffer + count + ptroffset));
                if (VErrFlag.BIT.IIcNoAckFlag)
                {
                    break;
                }
            }
            if (count < len)
            {
                break;
            }
            I2C_STOP();
            Delay_N_ms(WriteFMDelay_ms);

            WPEE0_ENABLE();
            ptroffset = ptroffset + len;
            size = size - len;
            WrLen = WrLen + len;
            if ((u8)(WrE2PageInAddr + len) < WrE2PageInAddr)
            {
                WrE2PageAddr++;
                if (WrE2PageAddr == 0)
                {
                    WrDeviceAddr |= 0x02; /*//high 16 bit address*/
                }
            }
            WrE2PageInAddr = WrE2PageInAddr + len;
        }
        if (size == 0)
        {
            VErrFlag.BIT.WrE2ErrFlag = 0;
        }
        else
        {
            I2C_STOP();
        }
        WPEE0_DISABLE();
#endif

        return (size);
    }
    /*
    //=========================================================
    //decription	::	get eeprom status
    //function		::	EEPROMGetState
    //Input			::	none
    //Output		::	0=right,1=read e2 error,2=write e2 error
    //call			::	none
    //effect		::	none
    //=========================================================
    */
    u8 EEPROMGetState(void)
    {
        u8 i = 0;
        if (VErrFlag.BIT.RdE2ErrFlag)
        {
            i |= 1;
        }
        if (VErrFlag.BIT.WrE2ErrFlag)
        {
            i |= 2;
        }
        return (i);
    }
    /*
    //================================================
    //Description     ::	read E2 data with check code
    //Function        ::	RdE2DataCRC
    //Input           ::	E2PgInAdr,ptr,len
    //Output          ::	if failure return 1 else 0
    //Call            ::	none
    //Effect          ::
    //===============================================
    */
    u8 RdE2DataCRC(u16 E2PgInAdr, u8 *ptr, u8 len)
    {
        u8 i;
        u8 Buff[16];
        if (len == 0 || len > 14 || ((E2PgInAdr & 0x000F) + len) > 15)
        {
            return (1);
        }
        for (i = 0; i < 3; i++)
        {
            EEPROMreader((E2PgInAdr & 0xFFF0) + (0x100 * i), &Buff[0], 16);
            if (VErrFlag.BIT.RdE2ErrFlag)
            {
                break;
            }
            if (CSandXOR_CheckGenerator(&Buff[0], 14))
            {
                continue;
            }
            COPYBYTEDATA((&Buff[0] + ((u8)(E2PgInAdr)&0x0F)), ptr, len); // copy data
            break;
        }
        if (VErrFlag.BIT.RdE2ErrFlag || i == 3)
        {
            return (1);
        }
        return (0);
    }
    /*
    //================================================
    //Description     ::	read E2 data with check code
    //Function        ::	RdE2DataCRC
    //Input           ::	E2PgInAdr,ptr,len
    //						VWrE2SafeByte =0x68
    //Output          ::	if failure return 1 else 0
    //Call            ::	none
    //Effect          ::
    //===============================================
    */
    u8 WrE2DataCRC(u16 E2PgInAdr, u8 *ptr, u8 len)
    {
        u8 i;
        u8 Buff[16];
        if (len == 0 || len > 14 || ((E2PgInAdr & 0x000F) + len) > 15)
        {
            VWrE2SafeByte = 0;
            return (1);
        }
        if (len != 14)
        {
            for (i = 0; i < 3; i++)
            {
                EEPROMreader((E2PgInAdr & 0xFFF0) + (0x100 * i), &Buff[0], 16);
                if (VErrFlag.BIT.RdE2ErrFlag)
                {
                    VWrE2SafeByte = 0;
                    return (1);
                }
                if (!(CSandXOR_CheckGenerator(&Buff[0], 14)))
                {
                    break;
                }
            }
        }
        COPYBYTEDATA(ptr, (&Buff[0] + ((u8)(E2PgInAdr)&0x0F)), len); // copy data
        CSandXOR_CheckGenerator(&Buff[0], 14);
        WPEE0_ENABLE();
        if (VWrE2SafeByte != Con_WrE2SafeByte)
        {
            WPEE0_DISABLE();
            return (1);
        }
        EEPROMwriter(E2PgInAdr & 0xFFF0, &Buff[0], 16);
        if (VErrFlag.BIT.WrE2ErrFlag)
        {
            return (1);
        }
        EEPROMreader(E2PgInAdr & 0xFFF0, &Buff[0], 16);
        if (VErrFlag.BIT.RdE2ErrFlag)
        {
            return (1);
        }
        if (CSandXOR_CheckGenerator(&Buff[0], 14))
        {
            return (1);
        }
        WPEE0_ENABLE();
        for (i = 1; i < 3; i++)
        {
            if (VWrE2SafeByte != Con_WrE2SafeByte)
            {
                break;
            }
            EEPROMwriter(((E2PgInAdr & 0xFFF0) + 0x100 * i), &Buff[0], 16);
            if (VErrFlag.BIT.WrE2ErrFlag)
            {
                break;
            }
            WPEE0_ENABLE();
        }
        VWrE2SafeByte = 0;
        WPEE0_DISABLE();
        if (i < 3)
        {
            return (1);
        }
        else
        {
            return (0);
        }
    }
    /*
     //=========================================================
     //decription	::	copy byte data from sptr to mptr
     //function		::	COPYBYTEDATA
     //Input			::	*sptr,*mptr
     //					len
     //Output		::	none
     //call			::	none
     //effect		::
     //=========================================================
     */
    void COPYBYTEDATA(u8 *sptr, u8 *mptr, u8 len)
    {
        u8 i;
        for (i = 0; i < len; i++)
        {
            *(mptr + i) = *(sptr + i);
        }
    }
    /*
    //=========================================================
    //Description     ::	data cs and xor check & generator
    //Function        ::	CSandXOR_CheckGenerator
    //Input           ::	ptr len
    //Output          ::	1= crc is error 0= crc is ok
    //Call            ::
    //Effect          ::
    //=========================================================
    */
    u8 CSandXOR_CheckGenerator(u8 *ptr, u8 len)
    {
        u8 i;
        u8 CSTemp1;
        u8 XORTemp2;

        CSTemp1 = 0;
        XORTemp2 = 0;

        for (i = 0; i < len; i++)
        {
            CSTemp1 = CSTemp1 + *(ptr + i);
            XORTemp2 = XORTemp2 ^ *(ptr + i);
        }
        if (CSTemp1 == *(ptr + len) && XORTemp2 == *(ptr + len + 1))
        {
            i = 0;
        }
        else
        {
            i = 1;
        }

        *(ptr + len) = CSTemp1;
        *(ptr + len + 1) = XORTemp2;

        return (i);
    }




/*********************************************************************************************************************************/
/**
 * @todo : 
 *          retrive first start byte, if the value not equal 0x55 save lookup table in eeprom
 *          - if  first start byte = 0x55 then its not first start 
 *          then system will go ahead with the retrived lookup table from eeprom and compare it with the existed one
 *          - if they are equal work with the existed one if not check the object that is not changed in size and base address
 *          and  retrive the data it size and base address changed neglect data
 *          - create queue for large sized object to be retrived asynchronously (its indices)
*/
#define FIRST_START_CHECK       0X55


static uint8_t write_enable=0;	/*is a control flag to start a periodic saving */
static uint8_t write_index = 0;	/*is the scheduled data runtime write index  */

static eeprom_instantaneous_t instantaneous_data_saving_buffer;


/**
 * @brief 
 *  Enable writing to EEPROM.
 * @param state 
 *  ENABLE: to start writing to EEPROM. 
 *  DISABLE: to disable writing to EEPROM. 
 */
void eeprom_set_write_status(uint8_t state)
{
    write_enable = state;
}


/**
 * @brief 
 *  to retrive the enable writing status.
 * @return uint8_t 
 *  ENABLE: it means that the writing process is in progress. 
 *  DISABLE: it means EEPROM in idle state.
 */
uint8_t eeprom_get_write_status(void)
{
    return write_enable;
}


/**
 * @brief 
 * it saves the lookup table in EEPROM.
 * @return uint8_t 
 */
uint8_t eeprom_save_lookup_table(void)
{
    uint16_t last_index = EEPROM_LOOKUP_TOTAL_SIZE;
    eeprom_set_write_status(ENABLE);
    EEPROMwriter(EEPROM_LOOKUP_TABLE_LAST_INDEX,&last_index,sizeof(last_index));
    EEPROMwriter(EEPROM_LOOKUP_TABLE_START_ADDRESS,&eeprom_lookup[0],sizeof(eeprom_lookup[0])*EEPROM_LOOKUP_TOTAL_SIZE);
}

/**
 * @brief 
 * it retrieves the lookup table from EEPROM.
 * @todo handle error codes
 * @return uint8_t 
 */
uint8_t eeprom_retrieve_lookup_table(void)
{
    eeprom_lookup_t eeprom_saved_lookup[EEPROM_LOOKUP_TOTAL_SIZE];
    uint16_t last_index = EEPROM_LOOKUP_TOTAL_SIZE;
    uint8_t start_ID= 0;
    EEPROMreader(EEPROM_FIRST_START_FLAG,&start_ID,sizeof(start_ID));
    if(FIRST_START_CHECK == start_ID) /*// not first start, first start check no. is already saved*/
    {
        /**TODO:: handle multiple checks **/
        EEPROMreader(EEPROM_LOOKUP_TABLE_LAST_INDEX,&last_index,sizeof(last_index));
        EEPROMreader(EEPROM_LOOKUP_TABLE_START_ADDRESS,&eeprom_saved_lookup[0],sizeof(eeprom_saved_lookup[0])*EEPROM_LOOKUP_TOTAL_SIZE);
		/* eeprom_read_rtc(); //read rtc from eeprom */
		PutTaskIntoQue(TASK_EEPROM_READ_BACK);

    }else // first start
    {
        eeprom_first_start();
    }
}


/**
 * @brief 
 * it is used to save all data when the power cut event occurs.
 * TODO:: handle is in the backup mode entrance. 
 */
void write_all_data_at_power_cut(void)
{
    write_index = 0;
    while (write_index < EEPROM_LOOKUP_TOTAL_SIZE)
    {
       dynamic_saving();
    }
     eeprom_save_lookup_table();

}


/*
//=========================================================
//Description     ::	write lookup table to eeprom
//Function        ::	eeprom_first_start
//Input           ::	
//Output          ::	
//Call            ::
//Effect          ::
//=========================================================
*/
void eeprom_first_start(void)
{
    uint16_t last_index = EEPROM_LOOKUP_TOTAL_SIZE;
	uint8_t first_start = FIRST_START_CHECK;
    EEPROMwriter(EEPROM_LOOKUP_TABLE_LAST_INDEX, &last_index, sizeof(last_index));
    EEPROMwriter(EEPROM_LOOKUP_TABLE_START_ADDRESS, &eeprom_lookup[0], sizeof(eeprom_lookup[0]) * EEPROM_LOOKUP_TOTAL_SIZE);
	EEPROMwriter(EEPROM_FIRST_START_FLAG,&first_start,sizeof(first_start));
	PutTaskIntoQue(TASK_EEPROM_CLEAR_DATA);
}

/*
//=========================================================
//Description     ::	assign 0 to all data location
//Function        ::	eeprom_clear_all_data
//Input           ::
//Output          ::
//Call            ::	EEPROMwriter
//Effect          ::
//=========================================================
*/
void eeprom_clear_all_data(void)
{
	for (int i = 0; i < EEPROM_LOOKUP_TOTAL_SIZE; i++)
	{
		eeprom_clear_object(i);
	}
}


/*
//=========================================================
//Description     ::	assign 0 to the selected object
//Function        ::	eeprom_clear_object
//Input           ::	index: object index in lookup table
//Output          ::
//Call            ::	EEPROMwriter
//Effect          ::
//=========================================================
*/
void eeprom_clear_object(eeprom_lookup_enum index)
{
	memset(eeprom_lookup[index].actual_address,0, eeprom_lookup[index].size);
	EEPROMwriter(eeprom_lookup[index].eeprom_address, eeprom_lookup[index].actual_address, eeprom_lookup[index].size);
}

void eeprom_clear_data_section(void){
	uint8_t temp =0;
	
	for(uint32_t i=EEPROM_FIRST_START_FLAG;i<EEPROM_LAST_ADDRESS;i++ )
	{
		EEPROMwriter(i,&temp,1);
	}
		
}
/*
//=========================================================
//Description     ::	assign eeprom address for each object in lookup table
//Function        ::	eeprom_init_saved_object
//Input           ::	
//Output          ::	error code (not implemented)
//Call            ::
//Effect          ::
//=========================================================
*/
uint8_t eeprom_init_saved_object(void)
{
	eeprom_lookup[0].eeprom_address = SAVED_DATA_START_ADDRESS;
	for (int i = 1; i < EEPROM_LOOKUP_TOTAL_SIZE; i++)
	{
		eeprom_lookup[i].eeprom_address = eeprom_lookup[i-1].eeprom_address
										 + eeprom_lookup[i-1].size;
	}
    return 0;
}

/*
//=========================================================
//Description     ::	assign eeprom address for each object in lookup table
//Function        ::	eeprom_retrieve_saved_objects
//Input           ::	
//Output          ::	error code (not implemented)
//Call            ::
//Effect          ::
//=========================================================
*/
uint8_t eeprom_retrieve_saved_objects(void)
{

    for (int i = 0; i < EEPROM_LOOKUP_TOTAL_SIZE; i++)
    {
        EEPROMreader(eeprom_lookup[i].eeprom_address,
                        eeprom_lookup[i].actual_address,
                        eeprom_lookup[i].size);
    }
    return 0;
}


/**
 * \brief : read rtc eeprom data and set to rtc only if the RTC is in factory reset 
 * 
 * \param : void
 * 
 * \return uint8_t
 */
/* uint8_t eeprom_read_rtc(void){
	rtc_get_date(RTC,&rtcdate.year,NULL,NULL,NULL);
	if(rtcdate.year < 2022){
		EEPROMreader(eeprom_lookup[EEPROM_UINT32_RTC_DAY].eeprom_address,eeprom_lookup[EEPROM_UINT32_RTC_DAY].actual_address,eeprom_lookup[EEPROM_UINT32_RTC_DAY].size);
		EEPROMreader(eeprom_lookup[EEPROM_UINT32_RTC_MON].eeprom_address,eeprom_lookup[EEPROM_UINT32_RTC_MON].actual_address,eeprom_lookup[EEPROM_UINT32_RTC_MON].size);
		EEPROMreader(eeprom_lookup[EEPROM_UINT32_RTC_YEAR].eeprom_address,eeprom_lookup[EEPROM_UINT32_RTC_YEAR].actual_address,eeprom_lookup[EEPROM_UINT32_RTC_YEAR].size);
		EEPROMreader(eeprom_lookup[EEPROM_UINT32_RTC_HOUR].eeprom_address,eeprom_lookup[EEPROM_UINT32_RTC_HOUR].actual_address,eeprom_lookup[EEPROM_UINT32_RTC_HOUR].size);
		EEPROMreader(eeprom_lookup[EEPROM_UINT32_RTC_MIN].eeprom_address,eeprom_lookup[EEPROM_UINT32_RTC_MIN].actual_address,eeprom_lookup[EEPROM_UINT32_RTC_MIN].size);
		EEPROMreader(eeprom_lookup[EEPROM_UINT32_RTC_SEC].eeprom_address,eeprom_lookup[EEPROM_UINT32_RTC_SEC].actual_address,eeprom_lookup[EEPROM_UINT32_RTC_SEC].size);
		EEPROMreader(eeprom_lookup[EEPROM_UINT32_RTC_WEEK].eeprom_address,eeprom_lookup[EEPROM_UINT32_RTC_WEEK].actual_address,eeprom_lookup[EEPROM_UINT32_RTC_WEEK].size);
		rtc_set_time(RTC , rtctime.hour , rtctime.minute, rtctime.second);
		rtc_set_date(RTC , rtcdate.year , rtcdate.month , rtcdate.day , rtcdate.week);		
	}
	    return 0;
	
} */

/*
//=========================================================
//Description     ::	synchronous function to save data to eeprom
//Function        ::	eeprom_instantaneous_save
//Input           ::	
//Output          ::	error code (not implemented)
//Call            ::
//Effect          ::
//=========================================================
*/
uint8_t eeprom_instantaneous_save(eeprom_lookup_enum index)
{
    EEPROMwriter(eeprom_lookup[index].eeprom_address,eeprom_lookup[index].actual_address,eeprom_lookup[index].size);
}

/*
//=========================================================
//Description     ::	assynchronous function to save data to eeprom
//Function        ::	eeprom_add_to_saving_queue
//Input           ::	
//Output          ::	error code (not implemented)
//Call            ::
//Effect          ::
//=========================================================
*/
uint8_t eeprom_add_to_saving_queue(eeprom_lookup_enum index)
{
    instantaneous_data_saving_buffer.object[instantaneous_data_saving_buffer.number_of_objects] = index;
    instantaneous_data_saving_buffer.number_of_objects++;
}

/**
 * @brief 
 * Dynamic saving process without any validation 
TODO:: Suggestion : 
 * make checksum on every 5 objects  
 */
void dynamic_saving(void)
{
  
    if (write_index < EEPROM_LOOKUP_TOTAL_SIZE)
    {
        EEPROMwriter(eeprom_lookup[write_index].eeprom_address,
                    eeprom_lookup[write_index].actual_address,
                    eeprom_lookup[write_index].size);
        if (!VErrFlag.BIT.WrE2ErrFlag)
        {
            write_index++;
        }
    }
    else
    {
        write_index=0;
        eeprom_set_write_status(DISABLE);
    }
    
}
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
