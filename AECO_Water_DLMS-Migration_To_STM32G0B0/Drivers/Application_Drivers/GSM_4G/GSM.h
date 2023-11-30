/**
 * @file GSM.h
 * @author Mohamed Ibrahem (mohamed.ibrahem@pylonump.com)
 * @brief 
 * @version 1.1
 * @date 2023-08-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef __GSM_H__
#define __GSM_H__

#include <stdint.h>

#pragma region GSM_SERIAL_INIT


#pragma endregion GSM_SERIAL_INIT

#define COMMAND_BUFF_SIZE			 250U 
#define TCP_THRRSHOLD_ERRORS         3U
#define MAX_LEN_VAR_AT_COMMAND  	 50U
#define MAX_LEN_VAR_NUMBER		      3U
#define MAX_SOCKETS					 12U
#define BUFF_MAX_SIZE				2500U
#define DISCONNECTION_TIME			6000U    /*12000U	disconnect timeout this threshold 12000x5m =	 1min **/

/***********************************************************Used Data types **************************************************************************/

typedef enum{
	NO_MODULE=0,
	BC65_MODULE=65,
	EC200U_MODULE=200
}MODULES_t;

typedef enum
{
	GSM_NO_OPERATION,
	GSM_MODULE_READY,
	GSM_MODULE_NUM,
	GSM_SIM_STATUS,
	GSM_REG_CS,
	GSM_REG_PS,
	GSM_GET_OPERATOR,
	GSM_CONFIG_PDP,
	GSM_ACT_PDP,
	GSM_GET_IP,
	GSM_OPEN_SERVER,
	GSM_CLOSE_SERVER,
	GSM_CONNECT_AS_CLIENT,
	GSM_SEND,
	GSM_SEND_ACK,
	GSM_RECEIVE,
	GSM_RECEIVE_ACK,
	GSM_OPEN_INCOMMING_CLIENT,
	GSM_SOCKET_SERVICE_STATUS,
	GSM_CLOSE_CONNECTION,
	GSM_DEACTIVATE_PDP,
	GSM_SIGNAL_QUALITY_DETECT,
	GSM_HANDLE_RECEIVED_MESSAGES,
	GSM_PING_REMOTE_SERVER,
	GSM_CONFIG_PDP_CLIENT_CONTEXT,
	GSM_ACTIVATE_PDP_CLIENT_CONTEXT,
	
}LTE_TCP_states_t;

typedef struct {
	uint32_t meter_cover_open_flag :1;
	uint32_t terminal_cover_open_flag :1;
	uint32_t magnetic_tamper_flag :1;
	uint32_t current_imbalence_flag :1;
	uint32_t low_battery_flag :1;
	uint32_t battery_removed_flag :1;
	uint32_t diffirential_current_flag :1;
	uint32_t reverse_current_L1_flag :1;
	uint32_t reverse_current_L2_flag :1;
	uint32_t reverse_current_L3_flag :1;
	uint32_t missing_phase_L1_flag :1;
	uint32_t missing_phase_L2_flag :1;
	uint32_t missing_phase_L3_flag :1;
	uint32_t neutral_current_flag :1;
	uint32_t power_overload_flag :1;
}alarm_bitaccess_t;


typedef union{
	uint32_t fullaccess;
	alarm_bitaccess_t alarm_bitaccess;
}tamper_alarm_flags_t;

typedef enum
{
	GSM_SOCKET_STATE_INITIAL,
	GSM_SOCKET_STATE_OPENING,
	GSM_SOCKET_STATE_CONNECTED,
	GSM_SOCKET_STATE_LISTENING,
	GSM_SOCKET_STATE_CLOSING
}socket_state_t;
typedef enum
{	GSM_SERVICE_TYPE_IDLE,
	GSM_SERVICE_TYPE_TCP,
	GSM_SERVICE_TYPE_UDP,
	GSM_SERVICE_TYPE_TCP_LISTENER,
	GSM_SERVICE_TYPE_TCP_INCOMING,
	GSM_SERVICE_TYPE_UDP_SERVICE,
	GSM_SERVICE_TYPE_UDP_INCOMING
}service_type_t;
typedef enum
{
	GSM_USB_MODEM,
	GSM_USB_AT,
	GSM_UART1,
	GSM_CMUX1,
	GSM_CMUX2,
	GSM_CMUX3,
	GSM_CMUX4,
	GSM_NAN
}at_port_t;
typedef enum
{
	GSM_BUFFER_ACCESS_MODE,
	GSM_DIRECT_PUSH_MODE,
	GSM_TRANSPARENT_ACCESS_MODE
}access_mode_t;

typedef struct __attribute__((__packed__))
{	uint8_t config_done;
	uint8_t required_connection;
	uint16_t disconnect_timeout;
	uint8_t error_counter ;
	uint8_t module_ID_Number ;
	uint8_t tampers_switch;
	uint8_t send_tamper;
	uint8_t remote_exist;
	uint8_t trails;
}gsm_attributes_t;


typedef struct __attribute__((__packed__))
{
	uint32_t remote_port;
	uint32_t local_port;
	uint8_t service_type;	/** TCP ,UDP ,TCP LISTENER ,TCP INCOMING ,UDP SERVICE ,UDP INCOMING **/
	uint8_t remote_IP[4];
	uint8_t socket_state; 	/** initial = 0, opening = 1, connected = 2, listening = 3, closing =4  **/
	uint8_t context_ID;		/**context ID range 1 -7 **/
	uint8_t server_ID;
	uint8_t access_mode;     /**buffer access = 0 , direct push = 1 , transparent = 2**/
	uint8_t AT_port;	/**"usbmodem" USB modem port, "usbat" USB AT port ,"uart1" UART port1 ,"cmux1" MUX port 1 ,"cmux2" MUX port 2 ,"cmux3" MUX port 3 ,"cmux4" MUX port 4**/
}GSM_socket_state_t;

 
/*******************************************************************************************************************************************************/





/********************************************************* Shared Variables******************************************************************************/
extern volatile  uint8_t read_flag; 
extern char apn_name[30];
extern volatile int8_t receive_connect_ID ;
extern volatile uint8_t execution_thd;
extern volatile uint8_t incomming_counter;
extern volatile tamper_alarm_flags_t tamper_alarm_flags;
extern char remote_server[30];
/********************************************************************************************************************************************************/




#pragma region SHARED_FUNCTIONS
void GSM_init(void);
void TCP_4G_encode(void);
void TCP_4G_decode(void);
void TCP_send_4G(uint8_t * buf,uint32_t size,uint8_t cid);
void GSM_configuration(void);
void module_reboot(void);
void GSM_request_socket_status(void);
void GSM_get_socket_status(void);
void GSM_disconnection_timeout(void);
void GSM_send_tampers(void);
void GSM_request_status(void);
void GSM_send_Ack(void);
void GSM_enable_remote_server_check(void);
void GSM_rx_Handler(uint8_t rec_data);
#pragma endregion SHARED_FUNCTIONS


#endif /*//__GSM_H__*/
