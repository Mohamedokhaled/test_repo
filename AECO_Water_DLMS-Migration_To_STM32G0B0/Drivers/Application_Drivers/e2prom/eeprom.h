#ifndef EEPROM_H_INCLUDED
#define EEPROM_H_INCLUDED

#include "E2define.h"
#include "stdint.h"
#include "stm32g0xx_hal.h"
#include "stdint.h"
#include "enums.h"
#include "eeprom_lookup.h"


#ifdef __cplusplus
extern "C" {
#endif


#define	u32 uint32_t
#define	u16 uint16_t
#define	u8  uint8_t
/*//#define	AT24C1024Device		0xA0*/

#define	WrE2_DeviceAddr		0xA0
#define	RdE2_DeviceAddr		0xA1


#define ByteDeley_us        5
#define WriteFMDelay_ms     5

#define	EEPROM_SDA_PORT		(0)
#define	EEPROM_SDA_MASK		((1<<24))
#define	EEPROM_SDA_PIN		24
#define	EEPROM_SCL_PORT		(0)
#define	EEPROM_SCL_MASK		((1<<25))
#define	EEPROM_SCL_PIN		25


#define	I2C_WP_Low()	0		/*//wp=low level*/
#define	I2C_WP_High()	1		/*//wp=high level*/
#define I2C_VDD_Out()   1       /*//Control_E2_VCC is out*/
#define I2C_VDD_High()  1       /*//E2_VCC =high level*/
#define I2C_VDD_Low()   0       /*//E2_VCC =low level*/

typedef	union
{
	u32	ErrFlag;
	struct
	{
		u32	RdRTCErrFlag	: 1;	/* //bit 0 */
		u32	WrRTCErrFlag	: 1;	/* //bit 1 */
		u32	IIcNoAckFlag	: 1;	/* //bit 2 */
		u32	RdE2ErrFlag		: 1;	/* //bit 3 */
		u32	WrE2ErrFlag		: 1;	/* //bit 4 */
		u32	RTCErrFlag		: 1;	/* //bit 5 */
		u32	CRCErrFlag		: 1; 	/* //bit 6 */
		u32	EnergyFlag		: 1;	/* //bit 7 */
		u32	E2promErrFlag	: 1;	/* //bit 8 */
		u32	E2ErrEventFlag	: 1;	/* //bit 9 */
		u32					:22;
	} BIT;
} ErrFlag_Struct;
	
typedef struct
{
	uint8_t* Data_ptr;
	uint16_t Address;
	uint8_t len;
}E2prom_periodicdata_t;	

/**status**/ 
#define NO_SPACE                0U
#define ALLOCATED_DONE          1U
#define READ_DONE_NO_ERROR      2U
#define READ_ERROR              3U
#define MEMORY_FULL             4U


typedef struct __attribute__((__packed__))
{
	eeprom_lookup_enum object[EEPROM_LOOKUP_TOTAL_SIZE];
	uint8_t number_of_objects;
}eeprom_instantaneous_t;





/* //--------------------------------------------------------- */
#define	Con_WrE2SafeByte	0x68
/* //========================================================= */
/* //--------------------------------------------------------- */
void	BSP_E2promBus_Init( void );
void	I2C_START( void );
void	I2C_STOP( void );
u8	Read_I2C_1ByteAck( void );
u8	Read_I2C_1ByteNoAck( void );
u8	Write_I2C_1Byte( u8 I2C_WR_data );
/*//---------------------------------------------------------*/
void WPEE0_ENABLE( void );
void WPEE0_DISABLE( void );
void EEPROMOpen ( void );
void EEPROMClose ( void );
void E2PROMInit( void );
u16 EEPROMreader( u32 offset, u8 *buffer, u16 size );
u16	EEPROMwriter( u32 offset, u8 *buffer, u16 size );
u8 EEPROMGetState( void );
u8	RdE2DataCRC( u16 offset, u8 *ptr, u8 len );
u8	WrE2DataCRC( u16 offset, u8 *ptr, u8 len );

void	Delay_N_us( uint16_t val );
void	Delay_N_ms( uint16_t val );
void	COPYBYTEDATA( u8 *sptr, u8 *mptr, u8 len );
u8	CSandXOR_CheckGenerator( u8 *ptr, u8 len );
/*//---------------------------------------------------------*/
extern	ErrFlag_Struct	VErrFlag;
extern	u8	VWrE2SafeByte;
//=========================================================

void eeprom_first_start(void);
void eeprom_clear_all_data(void);
void eeprom_clear_object(eeprom_lookup_enum index);
uint8_t eeprom_check_lookup_table_changed(void);

uint8_t eeprom_init_saved_object(void);
uint8_t eeprom_retrieve_saved_objects(void);

uint8_t eeprom_instantaneous_save(eeprom_lookup_enum index);
void eeprom_power_cut_save_periodicdata(void);
uint8_t eeprom_retrieve_lookup_table(void);
void write_all_data_at_power_cut(void);
void dynamic_saving(void);
uint8_t eeprom_get_write_status(void);
void eeprom_set_write_status(uint8_t state);
 

void eeprom_clear_data_section(void);





#ifdef __cplusplus
}
#endif


#endif /* eeprom_H_INCLUDED */
