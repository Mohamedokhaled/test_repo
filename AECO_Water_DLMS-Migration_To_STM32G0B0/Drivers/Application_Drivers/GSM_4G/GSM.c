/**
 * @file GSM.c
 * @author Mohamed Ibrahem (mohamed.ibrahem@pylonump.com)
 * @brief 
 * 		this file contains all 4G/GSM interfaces from module status to sockets configuration
 * it supports all .net sockets TCP/UDP services. 
 * @version 1.1
 * @date 2023-08-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "GSM.h"
#include "gpio.h"
#include "usart.h"
#include "hdlc_rx.h"
/*#include <stdio.h>*/

#pragma region EXEC_ATTR

volatile uint8_t execution_thd = 49; /**Execution threshold **/
volatile uint8_t incomming_counter = 0;
uint8_t response_reg_cs_denied_retries = 0;
volatile int8_t receive_connect_ID = -1;
volatile int8_t connect_ID = -1;
LTE_TCP_states_t TCP_state = GSM_MODULE_READY;
static uint8_t error_counter = 0;
char apn_name[30];
volatile tamper_alarm_flags_t tamper_alarm_flags = {0};
volatile tamper_alarm_flags_t previous_status_tampers = {0};
uint8_t exec_trials=0;
GSM_socket_state_t sockets_status[MAX_SOCKETS];
gsm_attributes_t gsm ={
						.config_done=0,
						.disconnect_timeout=DISCONNECTION_TIME,
						.error_counter=0,
						.module_ID_Number=NO_MODULE,
						.required_connection=GSM_SERVICE_TYPE_TCP_LISTENER,
						.tampers_switch = 1,
						.send_tamper=0,
						.remote_exist =10, /*10 trials if not exist postpone */
						.trails =10,
};
char remote_server[30];
char apn_name[30];
#pragma endregion EXEC_ATTR

#pragma region BUFFER_ATTR
/*buffer for command and it's head and tail		*/
volatile static uint16_t command_buff_head= 1;
volatile static uint16_t command_buff_tail = 0; 
volatile static uint8_t command_buff[COMMAND_BUFF_SIZE];
volatile uint16_t rec_startend_counter = 0; 
volatile uint8_t received_flag=0;
volatile  uint8_t read_flag=0; 
volatile static uint8_t temp = 0;
#pragma endregion BUFFER_ATTR

#pragma region GSM_REQ_RES

#pragma region GSM_RESPONSES

/*4G responses*/
 uint8_t response_OK[] = {"\r\nOK\r\n"};
 uint8_t response_data[] = {"\r\n+QIRD:      "}; /*spaces to be larger than any other at command --> temp == response_data[rec_data_indicator_pointer++]*/
 uint8_t response_ready[] = {"\r\nRDY\r\n"};
 uint8_t response_error[] = {"\r\nERROR\r\n"};
 uint8_t response_error_code[] = {"\r\n+CME ERROR:"};
 uint8_t response_sim_ready[] = {"\r\n+CPIN: READY\r\n"};
 uint8_t response_reg_cs_done[] = {"\r\n+CREG: 0,1\r\n"};
 uint8_t response_reg_cs_denied[] = {"\r\n+CREG: 0,3\r\n"};
 uint8_t response_reg_ps_4g[] = {"\r\n+CGREG: 0,1\r\n"};
 uint8_t response_reg_ps_NBIOT[] = {"\r\n+CEREG: 0,1\r\n"};
 uint8_t response_get_operator[] = {"\r\n+COPS: 0,2,\""};
 uint8_t response_get_ip_4g[] = {"\r\n+QIACT: 1,1,1,\"10.65.219.152\"\r\n"};
 uint8_t response_get_ip_NBIOT[] = {"\r\n+CGPADDR: 1,\"10.32.65.243\"\r\n"};
 uint8_t response_open_sockect[] = {"\r\n+QIOPEN: 0,0\r\n"};
 uint8_t response_open_sockect_fail[] = {"\r\n+QIOPEN: 0,5"};
 uint8_t response_sockets_state[] = {"\r\n+QISTATE: \r\n"};
 uint8_t response_sendok[] = {"\r\nSEND OK\r\n"};
 uint8_t response_sendfail[] = {"\r\nSEND FAIL\r\n"};
 uint8_t response_sendAck[] = {"\r\n+QISEND: 5,0,5\r\n"}; 
 uint8_t response_receive[] = {"+QIRD:"};
 uint8_t response_receiveAck[] = {"\r\n+QIRD: 0,0,0\r\n"};
 uint8_t response_signal_quality[] = {"\r\n+CSQ: "};
 uint8_t response_connect_as_client_4G[] = {"\r\nCONNECT"};	
 uint8_t response_connect_as_client_NBIOT[] = {"\r\n+CSQ: "};	
 uint8_t response_urc_connection_close[] = {"\r\n+QIURC: \"closed\","};
 uint8_t response_urc_incomming_data[] = {"\r\n+QIURC: \"recv\","};
 uint8_t response_urc_incomming_connection_full[] = {"+QIURC: \"incomming full\""};		
 uint8_t response_urc_incomming_connection[] = {"+QIURC: \"incomming\","};
 uint8_t response_urc_incomming_connection_full_NBIOT[] = {"\r\n+QIURC: \"incoming full\""};		
 uint8_t response_urc_incomming_connection_NBIOT[] = {"+QIURC: \"incoming\","};
 uint8_t response_start_send_flag[] = {"\r\n>"};
 uint8_t response_module_number_EC200[] ={"EC200U\r\n"};
/*const uint8_t response_module_number_EC200[] ="\r\nQuectel\r\nEC200U";*/
 uint8_t response_module_number_BC65[] ={"Quectel_Ltd\r\nQuectel_BC65"};
 uint8_t  response_urc_PDP_deactivate[] = {"+QIURC: \"pdpdeact\","};
 uint8_t TCP_service_type[] = {"TCP\""};
 uint8_t TCP_Listener_service_type[] = {"TCP LISTENER\""};	
 uint8_t TCP_Incoming_service_type[] = {"TCP INCOMING\""};
 uint8_t UDP_type[] = {"UDP\""};
 uint8_t UDP_service_type[] = {"UDP SERVICE\""};	
 uint8_t UDP_Incoming_service_type[] = {"UDP INCOMING\""};
/**"usbmodem" USB modem port, "usbat" USB AT port ,"uart1" UART port1 ,"cmux1" MUX port 1 ,"cmux2" MUX port 2 ,"cmux3" MUX port 3 ,"cmux4" MUX port 4**/
 uint8_t response_at_port_usbmodem[]={"\"usbmodem\""};
 uint8_t response_at_port_uart1[]={"\"uart1\""};
 uint8_t response_at_port_cmux1[]={"\"cmux1\""};
 uint8_t response_at_port_cmux2[]={"\"cmux2\""};
 uint8_t response_at_port_cmux3[]={"\"cmux3\""};
 uint8_t response_at_port_cmux4[]={"\"cmux4\""};
#pragma endregion GSM_RESPONSES




#pragma endregion GSM_REQ_RES

#pragma region LOCAL_FUCN_PROTOTYPE
void usart3_init(void);
void init_reboot_fun(void);
void GSM_rx_Handler(uint8_t data);
uint8_t check_reponse_in_recbuf(const uint8_t * str,uint8_t commandsize);
void GSM_clear_existed_response(uint16_t index,uint8_t size);
uint8_t GSM_find_connect_id(void);
uint16_t GSM_get_received_data_length(void);
void GSM_parameters_reset(void);
uint8_t insert_str_in_str(uint8_t* dist,uint8_t* src,uint8_t index_of_dis);
void insert_number2str(uint8_t* destination ,const uint8_t* source , uint8_t len , uint8_t number );
void GSM_send_tampers(void);
void GSM_reset_receive_buffer(void);


#pragma endregion LOCAL_FUCN_PROTOTYPE

#pragma region GSM_HW_INTERFACES

/**
 * @brief 
 * this function to initialize all needed  
 */
void GSM_init(void)
{
	 MX_USART6_UART_Init();
	 __HAL_UART_ENABLE_IT(&huart6, UART_IT_RXNE);
	module_reboot();
}


/**
 * @brief 
 * this function to reboot the 4G module and reset all execution parameters
 */
void module_reboot(void)
{
	/*reboot the module*/
	HAL_GPIO_WritePin(LTE_REBOOT_PIN_GPIO_Port,LTE_REBOOT_PIN_Pin,SET);
	HAL_Delay(300);
	HAL_GPIO_WritePin(LTE_REBOOT_PIN_GPIO_Port,LTE_REBOOT_PIN_Pin,RESET);
	GSM_parameters_reset();
}


/**
 * @brief 
 *  this function to handle GSM received messages
 * @param data 
 * 			: is the recevied character over UART from GSM module 
 */
void GSM_rx_Handler(uint8_t rec_data)
{
	if((rec_data>='0')||(rec_data<='z'))
   { command_buff[command_buff_head]=rec_data;
    command_buff_head=((command_buff_head+1)%COMMAND_BUFF_SIZE);
   }
	hdlc_rx_proc_4G(temp, HDLC_GSM_4G ,receive_connect_ID );
}

void GSM_send_data(uint8_t * pData,uint16_t size)
{
	HAL_UART_Transmit(&huart6,pData,size,HAL_MAX_DELAY);
}


#pragma endregion GSM_HW_INTERFACES

#pragma region ENCODING_DECODING
/**
 * @brief 
 * this function to handle the outgoing requests to 4G module.
 * it runs over UART to configure, request, and check status. 
 */
void TCP_4G_encode(void)
{
	
#pragma region GSM_REQUESTS
/**AT+QICSGP=<contextID>[,<context_type>,<APN>[,<username>,<password>)[,<authentication>]]]**/
	/**<context_type> Integer type. The protocol type.  IPv4=1  IPv6=2  IPv4v6 = 3*/
uint8_t request_config_PDP_NBIOT_server[50] = "AT+QICSGP=2,1,\"\",,,0\r\n";
uint8_t request_config_PDP_4G_server[50] =  "AT+QICSGP=1,1,\"\",\"\",\"\",0,0\r\n";
uint8_t request_config_PDP_4G_client[50] =  "AT+QICSGP=1,1,\"cws0345\",\"\",\"\",0\r\n";
uint8_t request_module_number[] = "AT+CGMM\r\n";
/*const uint8_t request_module_number[] = "ATI\r\n";*/
/*At_command to send	not a const cuz dma */
uint8_t request_sim_status[] = "AT+CPIN?\r\n";
uint8_t request_reg_cs[] = "AT+CREG?\r\n";
uint8_t request_reg_ps_4G[] = "AT+CGREG?\r\n";
uint8_t request_reg_ps_NBIOT[] = "AT+CEREG?\r\n";

uint8_t request_operator[] = "AT+COPS?\r\n";
/*const uint8_t request_config_PDP[] = "AT+QICSGP=1,1,\"PSAPN\",\"\",\"\",0,0\r\n";*/
/* const uint8_t request_config_PDP_NBIOT[] = "AT+QICSGP=2,1,\"PSIOT\",,,0\r\n";*/
/* const uint8_t request_config_PDP_4G[] = "AT+QICSGP=1,1,\"cws0345\",\"\",\"\",0,0\r\n";*/
uint8_t request_activate_PDP_4G[] = "AT+QIACT=1\r\n";
uint8_t request_activate_PDP_NBIOT[] = "AT+CGACT=1,1\r\n";

uint8_t request_IP[] =  "AT+QIACT?\r\n";
uint8_t request_IP_NBIOT[] =  "AT+CGPADDR=1\r\n";									  /*port , accesss buff , max client is 1 , connect is 100	*/
/*const uint8_t request_open_server[] = "AT+QIOPEN=1,0,\"TCP LISTENER\",\"10.32.64.110\",0,4059,0\r\n";*/
uint8_t request_open_server_4G[] = "AT+QIOPEN=1,0,\"TCP LISTENER\",\"127.0.0.1\",0,4059,0\r\n";
uint8_t request_open_server_NBIOT[] = "AT+QIOPEN=1,0,\"TCP LISTENER\",\"127.0.0.1\",1,4059,0\r\n";

 uint8_t request_close_server[] = "AT+QICLOSE=0\r\n";
/*AT+QIOPEN=<contextID>,<connectID>,<service_type>,"<IP_address>/<domain_name>",<remote_port>[,<local_port>[,<access_mode>[,<tcp_client_maxnum>[,<connect_timeout>]]]]*/
 uint8_t request_connect_as_client_4G[] = "AT+QIOPEN=1,0,\"TCP\",,0,0\r\n";
 uint8_t request_connect_as_client_NBIOT[] = "AT+QIOPEN=2,0,\"TCP\",,0,0\r\n";
 uint8_t request_send[] = "AT+QISEND=,\r\n";
 uint8_t request_send_ACK[] = "AT+QISEND=,0\r\n";
 uint8_t request_receive[] = "AT+QIRD=,1500\r\n";
 uint8_t request_receive_ACK[] = "AT+QIRD=,0\r\n";
 uint8_t request_state[] = "AT+QISTATE=1,0\r\n";
 uint8_t request_close_connection[] = "AT+QICLOSE=\r\n";
 uint8_t request_deactivate_PDP[] = "AT+QIDEACT=1\r\n";
 uint8_t request_signal_quality_detect[] = "AT+CSQ\r\n";
 uint8_t request_power_off[] = "AT+QPOWD\r\n";

#pragma endregion GSM_REQUESTS
	exec_trials--;
		switch(TCP_state)
		{
			case GSM_MODULE_NUM :
			{
				GSM_send_data(request_module_number,9);

				gsm.module_ID_Number=0;
			}
			break;
			case GSM_SIM_STATUS :
			{
				GSM_send_data(request_sim_status,10);
			}
			break;
			case GSM_REG_CS :
			{
				GSM_send_data(request_reg_cs,10);
			}
			break;
			case GSM_REG_PS:
			{
				if (gsm.module_ID_Number==EC200U_MODULE)
				{
					GSM_send_data(request_reg_ps_4G,11);
				}
				else /**(gsm.module_ID_Number==BC65_MODULE)**/
				{
					GSM_send_data(request_reg_ps_NBIOT,11);
					
				}

			}
			break;
			case GSM_CONFIG_PDP :
			{
				uint8_t size_st=strlen(apn_name);
				if (gsm.module_ID_Number==EC200U_MODULE)
				{
					insert_str_in_str(request_config_PDP_4G_server,apn_name,15);
					GSM_send_data(request_config_PDP_4G_server,strlen(request_config_PDP_4G_server));
					/*GSM_send_data(request_config_PDP_4G_client,strlen(request_config_PDP_4G_client));*/
					
				}
				else
				{
					insert_str_in_str(request_config_PDP_NBIOT_server,apn_name,15);
					GSM_send_data(request_config_PDP_NBIOT_server,strlen(request_config_PDP_NBIOT_server));
				}
					gsm.config_done =1;
			}
			break;
			case GSM_ACT_PDP :
			{
				if (gsm.module_ID_Number==EC200U_MODULE)
				{
					GSM_send_data(request_activate_PDP_4G,strlen(request_activate_PDP_4G));
				}
				else
				{
					GSM_send_data(request_activate_PDP_NBIOT,strlen(request_activate_PDP_NBIOT));
				}
					gsm.config_done =1;
			}
			break;	
			case GSM_OPEN_SERVER :
			{
				if(gsm.module_ID_Number==EC200U_MODULE)
				{
					GSM_send_data(request_open_server_4G,strlen(request_open_server_4G));
				}
				else
				{
					GSM_send_data(request_open_server_NBIOT,strlen(request_open_server_NBIOT));

				}
				gsm.config_done = 1;
				gsm.trails =10; 

			}
			break;
			case GSM_CLOSE_SERVER :
			{
					GSM_send_data(request_close_server,strlen(request_close_server));
					/*	TCP_state =GSM_HANDLE_RECEIVED_MESSAGES;*/
			}
			break;
			case GSM_RECEIVE :
			{
				if((receive_connect_ID<MAX_SOCKETS )&& (receive_connect_ID>0))
				{
					 uint8_t tem[MAX_LEN_VAR_AT_COMMAND]={0};
					insert_number2str(tem ,request_receive, 8 , receive_connect_ID );
					GSM_send_data(tem, strlen(tem)); 
					read_flag=1;
					TCP_state = GSM_HANDLE_RECEIVED_MESSAGES;
				}
				if(0==exec_trials)
				{
					TCP_state = GSM_HANDLE_RECEIVED_MESSAGES;
				}
			}
			break;
			case GSM_CLOSE_CONNECTION:
			{
				 uint8_t tem[MAX_LEN_VAR_AT_COMMAND]={0};
				insert_number2str(tem ,request_close_connection, 11 , receive_connect_ID );	
				if((receive_connect_ID<MAX_SOCKETS )&& (receive_connect_ID>0))
				{																											
					GSM_send_data(&tem, strlen(&tem));
					TCP_state = GSM_HANDLE_RECEIVED_MESSAGES;
					gsm.config_done =0;
					/**reboot*/
				}
				else if (0 == receive_connect_ID)
				{
					GSM_send_data(tem, strlen(tem));
					module_reboot();
					gsm.config_done =0;
				}
			}
			break;
			
			case GSM_SOCKET_SERVICE_STATUS:
			{
				GSM_send_data(request_state,strlen(request_state));
				/*TCP_state =GSM_HANDLE_RECEIVED_MESSAGES;*/
					if (0 == gsm.trails)
					{
						module_reboot();
					}
					gsm.trails --;

			}
			break;
			case GSM_CONFIG_PDP_CLIENT_CONTEXT:
			{
				if (gsm.module_ID_Number==EC200U_MODULE)
				{
					GSM_send_data(request_config_PDP_4G_client,strlen(request_config_PDP_4G_client));
					
				}
				else
				{
					GSM_send_data(request_config_PDP_4G_client,strlen(request_config_PDP_4G_client));
				}
					gsm.config_done =1;
				

			}
			break;
			case GSM_ACTIVATE_PDP_CLIENT_CONTEXT:
			{
				 uint8_t request_activate_PDP_4G_client[] = "AT+QIACT=1\r\n";
				if (gsm.module_ID_Number==EC200U_MODULE)
				{
					GSM_send_data(request_activate_PDP_4G_client,strlen(request_activate_PDP_4G_client));
				}
				else
				{
					GSM_send_data(request_activate_PDP_NBIOT,strlen(request_activate_PDP_NBIOT));
				}
					gsm.config_done =1;
			}
			break;
			case GSM_CONNECT_AS_CLIENT:
			{
				uint8_t tem[MAX_LEN_VAR_AT_COMMAND]={0};
					
				gsm.remote_exist --;
				if (255 == gsm.remote_exist )
				{
					gsm.remote_exist = 0;
					gsm.required_connection = GSM_SERVICE_TYPE_TCP_LISTENER;
				}
				if(gsm.module_ID_Number==EC200U_MODULE)
				{
					insert_str_in_str(request_connect_as_client_4G,remote_server,20);
					GSM_send_data(request_connect_as_client_4G,strlen(request_connect_as_client_4G));
				}
				else
				{
					insert_str_in_str(request_open_server_NBIOT,remote_server,20);
					GSM_send_data(request_open_server_NBIOT,strlen(request_open_server_NBIOT));
				}
				gsm.config_done =1;
				gsm.trails =10; 
			}
			break;
			case GSM_SEND_ACK:
			{
				GSM_send_data(request_send,strlen(request_send));
				GSM_send_data("meter ID1 ,tammper",18);
			}
			break;
			default:
			break;
		}

}
/**
 * @brief 
 * this function to handle received responses.
 * it runs over UART to handle received data from 4G module 
 * TODO:: @ibrahem:we need to handle timeout 
 */
void  TCP_4G_decode(void)
{
		switch(TCP_state)
		{
			case GSM_MODULE_READY :
			{
				/**check the sim is ready or not **/
				if (check_reponse_in_recbuf(&response_ready[2],3))
				{
						TCP_state =GSM_MODULE_NUM;
				}		
				/**set encoding invocation period threshold **/
				execution_thd = 49;
			}
			break;
			case GSM_MODULE_NUM:
			{
					/**check the module part number **/
				if(check_reponse_in_recbuf(&response_module_number_EC200[0] , 5))
				{
					gsm.module_ID_Number=EC200U_MODULE;
					TCP_state = GSM_SIM_STATUS ;
				}
				else if(check_reponse_in_recbuf(&response_module_number_BC65[13] , 12))
				{
					gsm.module_ID_Number=BC65_MODULE;
					TCP_state = GSM_SIM_STATUS ;
				}
				else
				{
					/*TCP_state = MODULE_READY ;*/
				}

			}
			break;
			case GSM_SIM_STATUS :
			{
				/**check if the sim is ready or not **/
				if (check_reponse_in_recbuf(&response_sim_ready[3],11))
				{
					if (gsm.module_ID_Number==EC200U_MODULE)
					{
						TCP_state = GSM_REG_CS ;
					}
					else
					{
						TCP_state = GSM_REG_PS ;
					}
				}
			}
			break;
			case GSM_REG_CS :
			{
				/**check the sim is registered as a 4G or not **/
				/**TODO:: we have to handle if the access denid all the time we have to procceed to the next **/
				if (check_reponse_in_recbuf(&response_reg_cs_done[3],9))
				{
					TCP_state =GSM_REG_PS;
				}
			}
			break;
			case GSM_REG_PS:
			{
				/**check the sim is localized like a GSM or not **/
				if (EC200U_MODULE == gsm.module_ID_Number)
				{
					if (check_reponse_in_recbuf(&response_reg_ps_4g[3],10))
					{	
						if (GSM_SERVICE_TYPE_TCP_LISTENER == gsm.required_connection )
						{
							TCP_state =GSM_CONFIG_PDP;
						}
						else if (GSM_SERVICE_TYPE_TCP == gsm.required_connection)
						{
							TCP_state =GSM_CONFIG_PDP_CLIENT_CONTEXT;
						}
						else
						{

						}
						
					}
				}
				else if (BC65_MODULE == gsm.module_ID_Number)
				{
					if (check_reponse_in_recbuf(&response_reg_ps_NBIOT[3],10))
					{
						if (GSM_SERVICE_TYPE_TCP_LISTENER == gsm.required_connection )
						{
							TCP_state =GSM_CONFIG_PDP;
						}
						else if (GSM_SERVICE_TYPE_TCP == gsm.required_connection)
						{
							TCP_state = GSM_CONFIG_PDP_CLIENT_CONTEXT;
						}
						else
						{
							
						}
					}
				}
			}
			break;
			case GSM_CONFIG_PDP :
			{
				if (1 == gsm.config_done)
				{
					/**check the PDP is configured or not **/
					if (check_reponse_in_recbuf(&response_OK[2],2))
					{
						TCP_state =GSM_ACT_PDP;
						gsm.config_done =0; 
					}
				}
				
			}
			break;
			case GSM_ACT_PDP :
			{
				if (1 == gsm.config_done)
				{
					/**check the PDP is activated or not **/
					if (check_reponse_in_recbuf(&response_OK[2],2))
					{
						TCP_state =GSM_OPEN_SERVER;
						/*TCP_state =GSM_CONNECT_AS_CLIENT;*/
						gsm.config_done =0;
					}
				}
			}
			break;	
			case GSM_OPEN_SERVER :
			{
				if (1 == gsm.config_done)
				{
					/**check the socket is opened or not **/
					if (check_reponse_in_recbuf(&response_open_sockect[2],12))
					{
						TCP_state =GSM_SOCKET_SERVICE_STATUS;
						/*TCP_state = GSM_CONFIG_PDP_CLIENT_CONTEXT;*/
						/**increase invocation times **/
						execution_thd =0;
						gsm.config_done =0;
						 GSM_reset_receive_buffer();

					}
					else if (check_reponse_in_recbuf(&response_open_sockect_fail[2],12))
					{
						TCP_state =GSM_MODULE_READY;
						module_reboot();
					}
					
				}

			}
			break;
			case GSM_CLOSE_SERVER :
			{
				/**check the socket is closed or not **/
				if (check_reponse_in_recbuf(&response_OK[2],2))			
				{
					module_reboot();	
				}
				
			}
			break;
			case GSM_RECEIVE :
			{
				 if(check_reponse_in_recbuf(&response_urc_incomming_data[10], 4))
				{
					read_flag=1;
					incomming_counter = 0; /**reset incomming full flag counter **/
					connect_ID = GSM_find_connect_id();
					/*//save id in buff*/
/* 					if(receive_connect_ID == -1)*/
/* 					{*/
						receive_connect_ID = connect_ID;
					/*}	*/
					TCP_state =GSM_RECEIVE;
					exec_trials =10;
				}
			}
			break;
			case GSM_CLOSE_CONNECTION:
			{
				if (check_reponse_in_recbuf(&response_OK[2],2))
				{
						module_reboot();
					/*gsm.required_connection = GSM_SERVICE_TYPE_TCP_LISTENER;*/
				}	
			}
			break;
			case GSM_CONFIG_PDP_CLIENT_CONTEXT:
			{
				if (1 == gsm.config_done)
				{
					if (check_reponse_in_recbuf(&response_OK[2],2))
					{
						TCP_state = GSM_ACTIVATE_PDP_CLIENT_CONTEXT ;
						gsm.config_done = 0;
					}	
				}
			}
			break;
			case GSM_ACTIVATE_PDP_CLIENT_CONTEXT:
			{
				if (1 == gsm.config_done)
				{
					if (check_reponse_in_recbuf(&response_OK[2],2))
					{
						TCP_state = GSM_CONNECT_AS_CLIENT ;
						gsm.config_done = 0;
					}	
				}
			}
			break;
			case GSM_CONNECT_AS_CLIENT:
			{
				if (1 == gsm.config_done)
				{
					/** client connected  socket 
					 *  TODO:: handle states jumping for client scenario **/
					if (check_reponse_in_recbuf(&response_open_sockect[2],10))
					{
						/*TCP_state=GSM_SEND;not handled yet */
						TCP_state =GSM_SOCKET_SERVICE_STATUS;
						execution_thd = 49;
						gsm.config_done = 0;
						GSM_reset_receive_buffer();
					}	
					else if (check_reponse_in_recbuf(&response_open_sockect_fail[2],12))
					{
						
						module_reboot();
						gsm.config_done = 0;
					
						
					}
					sockets_status[0].service_type=GSM_SERVICE_TYPE_IDLE;
				}
			}
			break;
			
			case GSM_SEND:
			{
				if (1 == gsm.config_done)
				{
					if (check_reponse_in_recbuf(&response_sendok[2],7));
					{
						if(GSM_SERVICE_TYPE_TCP == gsm.required_connection)
						{
							TCP_state = GSM_CLOSE_CONNECTION;
							gsm.required_connection=GSM_SERVICE_TYPE_TCP_LISTENER;
							previous_status_tampers.fullaccess=tamper_alarm_flags.fullaccess;
							sockets_status[0].service_type= GSM_SERVICE_TYPE_IDLE;
							sockets_status[0].context_ID=0;
							gsm.tampers_switch = 1;
							gsm.config_done =1;
							receive_connect_ID = 0;
						}
					}
				}
				if (check_reponse_in_recbuf(response_start_send_flag,3));
				{
					
						
						/*TCP_state =GSM_SEND;*/
					gsm.config_done =1;
					gsm.send_tamper =1;
					
				}
			}
			
			break;
			case GSM_SOCKET_SERVICE_STATUS:
			{
				GSM_get_socket_status();
			}
			break;
			case GSM_HANDLE_RECEIVED_MESSAGES:
			{
				/**QUIRC recv : **/
				if(check_reponse_in_recbuf(&response_urc_incomming_data[10], 4))
				{
					read_flag=1;
					incomming_counter = 0; /**reset incomming full flag counter **/
					connect_ID = GSM_find_connect_id();
					/*save id in buff*/
					/* 					if(receive_connect_ID == -1)*/
					/* 					{*/
					receive_connect_ID = connect_ID;
					/*}*/
					TCP_state =GSM_RECEIVE;
					exec_trials =10;
				}
				/** ERROR **/
				if(check_reponse_in_recbuf(&response_error[2]  , 5)||check_reponse_in_recbuf(&response_error_code[3]  , 10))
				{
					/**increment error till to reach max then reboot**/
					gsm.error_counter++;
					/*if exceed the threshold no. of errors*/
					if(gsm.error_counter > TCP_THRRSHOLD_ERRORS)
					{
						module_reboot();
					}
				}
				/**get data length**/
				if(check_reponse_in_recbuf(&response_receive[1]  , 5))
				{
					/**get data length**/
					/* uint16_t data_len=GSM_get_received_data_length();*/
					/**close receive buffer **/
					read_flag=0;
				}
				/** incomming full**/
				if(check_reponse_in_recbuf(&response_urc_incomming_connection_full ,20))
				{
					incomming_counter++;
					if (incomming_counter>2)
					{
						module_reboot();
					}
					
				}
				/**PDP deactivate **/
				if (check_reponse_in_recbuf(&response_urc_PDP_deactivate,19))
				{
					TCP_state=GSM_ACT_PDP;
				}
				/**+QIURC: "closed",**/
				if (check_reponse_in_recbuf(&response_urc_connection_close[3],14))
				{
					connect_ID = GSM_find_connect_id();
					/*save id in buff*/
					if(receive_connect_ID == -1)
					{
						receive_connect_ID = connect_ID;
					}
					TCP_state=GSM_CLOSE_CONNECTION;
					gsm.config_done =1;
				}
				/**RDY**/
				if (check_reponse_in_recbuf(&response_ready[2],3))
				{
					TCP_state =GSM_MODULE_NUM;
				}
				else
				{

				}
				if (check_reponse_in_recbuf(&response_open_sockect_fail[2],12))
				{
					module_reboot();
				}
				
				GSM_send_tampers();
			}
			break;
			default:

			break;
		}

}
#pragma endregion ENCODING_DECODING

#pragma region UTILITIES_FUNCTION
/**
 * @brief 
 * this function to confugure 4G module to disble ECO mode and save the configuration to the module.
 */
void GSM_configuration(void)
{
	
	GSM_send_data("ATE0\r\n",6);
	delay_ms(300);
	GSM_send_data("AT&W\r\n",6);
	delay_ms(300);
	module_reboot();
}

/**
 * @brief 
 * 	this function to search for a response in the receive buffer . 
 * @param str 
 * 			 : is what we are looking for in the receive buffer. 
 * @param commandsize 
 * 			 : is the string size 
 * @return uint8_t 
 * 			 : it return 1  >>  in case the string is exist. 
 *			 : it return 0  >>  in case the string isn't exist. 
 * 					
 */
uint8_t check_reponse_in_recbuf(const uint8_t * str,uint8_t commandsize)
{
	uint8_t i=0;
	uint8_t found=0;
	uint16_t start_index=command_buff_tail;
	while(!found)
		{
			if (str[i]==command_buff[start_index++])
			{
				if (i==(commandsize-1))
				{
					found=1;
					command_buff_tail=start_index;
					GSM_clear_existed_response(start_index,commandsize);
 					break;
				}
				i++;
			}
			else
			{
				i=0;
			}
			if (start_index==COMMAND_BUFF_SIZE)
			{
				start_index=0;
			}
			if (start_index==command_buff_head)
			{
				break;
			}
		}
	return found;
}

/**
 * @brief 
 * this function to clear messages after handling 
 * @param index 
 * 	the last index of the existed response 
 * @param size 
 * the size of the message itself
 */
void GSM_clear_existed_response(uint16_t index,uint8_t size)
{
	for (uint16_t i = 0; i <size; i++)
	{
		command_buff[index--]=0;
		if (index==255)
		{
			index = COMMAND_BUFF_SIZE;
		}
	}
}

/**
 * @brief 
 * this function to get the connect ID from received messages.
 * @return uint8_t 
 * it returns the connected ID on range 1-255
 */
uint8_t GSM_find_connect_id(void)
{
	uint8_t id=0;
	 for (uint8_t i = 0; i < 8; i++)
	 {
		 if (command_buff[command_buff_tail]==',')
		{
			/**left with no code to skip this iteration **/
		}
		else if (command_buff[command_buff_tail]==' ')
		{
			/**left with no code to skip this iteration **/
		}
		else if (command_buff[command_buff_tail]=='\r')
		{
			command_buff_tail++;
			break;
		}
		else if ((command_buff[command_buff_tail]>='0')&&(command_buff[command_buff_tail]<='9'))
		{
			id=(id*10)+(command_buff[command_buff_tail]-'0');
		}
		command_buff_tail++;
		if (command_buff_tail==COMMAND_BUFF_SIZE)
		{
			command_buff_tail=0;
		}
 	 }	
	return id;
}

/**
 * @brief 
 * this function to get data length from received response.
 * @return uint16_t 
 * it returns the data length in range 1-65535
 */
uint16_t GSM_get_received_data_length(void)
{
	uint16_t data_size=0;
	 for (uint8_t i = 0; i < 10; i++)
	 {
		if (command_buff[command_buff_tail]==' ')
		{
			/**left with no code to skip this iteration **/
		}
		else if (command_buff[command_buff_tail]=='\r')
		{
			break;
		}
		else if ((command_buff[command_buff_tail]>='0')&&(command_buff[command_buff_tail]<='9'))
		{
			data_size=(data_size*10)+(command_buff[command_buff_tail]-'0');
		}
		command_buff_tail++;
		if (command_buff_tail==COMMAND_BUFF_SIZE)
		{
			command_buff_tail=0;
		}
 	 }	
	return data_size;
}


/**
 * @brief 
 * this function to reset GSM general parameters . 
 */
void GSM_parameters_reset(void)
{
	gsm.error_counter = 0;
	for (uint16_t i = 0; i < COMMAND_BUFF_SIZE; i++)
	{
		command_buff[i] = 0;
	}
	command_buff_tail = 0;
	command_buff_head = 1;
	read_flag = 0;
	receive_connect_ID =-1;
	TCP_state = GSM_MODULE_READY;
}
/**
* @brief
* 	this function to insert a string inside another string
* @param src
* this is the inserted string
* @param dist
* this is the destination string that will contain all data  
* @param index
* it the start index of the inserted data
* @return uint8_t
* TODO:: handele error codes  
* it will hold an error code in the future .
*/
uint8_t insert_str_in_str(uint8_t* dist,uint8_t* src,uint8_t index_of_dis)
{
	uint8_t i = 0;
	uint8_t totalStr[50]={0};
	memcpy(totalStr,dist,index_of_dis);
	for ( i = 0; src[i]!='\0'; i++)
	{
		totalStr[index_of_dis+i]=src[i];
	}
	i+=index_of_dis;
	for (uint8_t ind = 0; dist[index_of_dis+ind]!='\0'; ind++)
	{
		totalStr[i++]=dist[index_of_dis+ind];
	}
	memcpy(dist,totalStr,i);
}

/**
* @brief
* this function to insert number into a string  
* @param destination
* this is the destination string that will contain all data 
* @param source
* this is the string that will concatenated with the passed number to be fill together in the destination string 
* @param len
* this is the insert index of the number in the string 
* @param number
* this is the actual number to be inserted. 
*/
void insert_number2str(uint8_t* destination ,const uint8_t* source , uint8_t len , uint8_t number )
{
	int8_t i=0 ;
	uint8_t temp_str[MAX_LEN_VAR_AT_COMMAND];
	uint8_t string_number[MAX_LEN_VAR_NUMBER] = "0";
	/*copy first part*/
	strncpy(destination , source , len);
	/*hold second in temp string*/
	strcpy(temp_str , source+len );
	/*end the first string*/
	destination[len] = '\0';
	/*convert no. and concatenate it*/
	if(number == 0)
	{
		strcat(destination , string_number);
	}
	else
	{
		while(number > 0)
		{
			string_number[i] = number% 10 + '0';
			number = number / 10;
			i++;
		}
		for(i=i-1 ; i>=0 ; i--)
		{
			destination[len++] = string_number[i];
		}
		destination[len] = '\0';
	}
	/*copy the remain part*/
	strcat(destination, temp_str);
}
/**
 * @brief 
 * this function to send frames 
 * @param buf 
 * the data that need to be sent
 * @param size 
 *  data frame size 
 * @param cid 
 * connected ID of the current connection 
 */
void TCP_send_4G(uint8_t * buf,uint32_t size,uint8_t cid)
{
	uint8_t request_send[] = "AT+QISEND=,\r\n";
	uint8_t temp_string[MAX_LEN_VAR_AT_COMMAND];
	/*add connect ID*/
	insert_number2str(temp_string ,request_send, 10 , connect_ID );
	receive_connect_ID=-1;
	/*add len of send buff*/
	insert_number2str(temp_string ,temp_string, strlen(temp_string)-2 , size);
	GSM_send_data(temp_string,strlen(temp_string));
	GSM_send_data(buf,size);
	/**set timeout Here to be checked when we switch from server to client **/
	gsm.disconnect_timeout = DISCONNECTION_TIME;
}
/**
 * @brief 
 * This function to get the socket connected ID
 * @return uint8_t 
 * range 0-11
 */
uint8_t GSM_get_state_connected_ID(void)
{
	uint8_t id=0;
	 for (uint8_t i = 0; i < 8; i++)
	 {
		if (command_buff[command_buff_tail]==' ')
		{
			/**left with no code to skip this iteration **/
		}
		else if (command_buff[command_buff_tail]==',')
		{
			command_buff_tail++;
			break;
		}
		else if ((command_buff[command_buff_tail]>='0')&&(command_buff[command_buff_tail]<='9'))
		{
			id=(id*10)+(command_buff[command_buff_tail]-'0');
		}
		command_buff_tail++;
		if (command_buff_tail==COMMAND_BUFF_SIZE)
		{
			command_buff_tail=0;
		}
 	 }	
	return id;
}
/**
 * @brief 
 * This function to get the service type 
 * @return uint8_t 
 * it retuen one of the service type due to service type enum
 * 	TCP,
 * 	UDP,
 * 	TCP_LISTENER,
 * 	TCP_INCOMING,
 * 	UDP_SERVICE,
 * 	UDP_INCOMING
 */
uint8_t GSM_get_state_service_type(void)
{
	uint8_t exit = 0;
	uint8_t service_state = GSM_SERVICE_TYPE_IDLE;
	for (uint8_t i = 0; i < 3; i++)
	{
		if (command_buff[command_buff_tail++]=='"')
		{
			switch (command_buff[command_buff_tail])
			{
				case 'T':
					{
						if (check_reponse_in_recbuf(&TCP_Listener_service_type[0],13))
						{
							service_state =GSM_SERVICE_TYPE_TCP_LISTENER;
						}
						else if (check_reponse_in_recbuf(&TCP_Incoming_service_type[0],13))
						{
							service_state =GSM_SERVICE_TYPE_TCP_INCOMING;
						}
						else if (check_reponse_in_recbuf(&TCP_service_type[0],4))
						{
							service_state =GSM_SERVICE_TYPE_TCP;
						}
						else
						{

						}
					}
					break;
				case 'U':
					{
						if (check_reponse_in_recbuf(&UDP_Incoming_service_type[0],13))
						{
							service_state =GSM_SERVICE_TYPE_UDP_INCOMING;
						}
						else if (check_reponse_in_recbuf(&UDP_service_type[0],12))
						{
							service_state =GSM_SERVICE_TYPE_UDP_SERVICE;
						}
						else if (check_reponse_in_recbuf(&UDP_type[0],4))
						{
							service_state =GSM_SERVICE_TYPE_UDP;
						}
						else
						{

						}
					}
					break;
				default:
					break;
			}
			exit = 1;
		}
		if (1==exit)
		{
			break;
		}
	}
	return service_state;
}
/**
 * @brief 
 * this function to get the service IP 
 * @param IP 
 * this is the pointer to an array of 4 members to be filled inside the function.
 */
void GSM_get_state_IP(uint8_t *IP)
{
	uint8_t number_of_Dots=0;
	
	for (uint8_t i = 0; i < 25; i++)
	{
		if ((number_of_Dots>=3)&&(command_buff[command_buff_tail]=='"'))
		{
			command_buff_tail+=2;
			break;
		}
		else if (command_buff[command_buff_tail]=='.')
		{
			number_of_Dots++;
			IP[number_of_Dots]=0;
		}
		else if ((command_buff[command_buff_tail]>='0')&&(command_buff[command_buff_tail]<='9'))
		{
			IP[number_of_Dots]=(IP[number_of_Dots]*10)+(command_buff[command_buff_tail]-'0');
		}
		command_buff_tail++;
		if (command_buff_tail==COMMAND_BUFF_SIZE)
		{
			command_buff_tail=0;
		}	
	}
}
/**
 * @brief 
 * this functin to get the required number from the buffer .
 * @return uint32_t 
 *  it supports to return values in range up to 4 bytes .
 */
uint32_t GSM_get_number_32Bit(void)
{
	uint32_t num=0;
	 for (uint8_t i = 0; i < 8; i++)
	 {
		if ( command_buff[command_buff_tail] == ' ')
		{
			/**left with no code to skip this iteration **/
		}
		else if ( command_buff[command_buff_tail] == ',' )
		{
			command_buff_tail++;
			break;
		}
		else if (( command_buff[command_buff_tail] >= '0') && ( command_buff[command_buff_tail] <= '9' ))
		{
			num = ( num * 10) + ( command_buff[command_buff_tail] - '0' );
		}
		command_buff_tail++;
		if ( COMMAND_BUFF_SIZE == command_buff_tail )
		{
			command_buff_tail = 0;
		}
 	 }	
	return num;
}
/**
 * @brief 
 * this function to return the AT port interface 
 * @return uint8_t 
 * GSM_USB_MODEM,
 * GSM_USB_AT,
 * GSM_UART1,
 * GSM_CMUX1,
 * GSM_CMUX2,
 * GSM_CMUX3,
 * GSM_CMUX4,
 * 
 */
uint8_t GSM_get_at_port(void)
{
	uint8_t at_port = GSM_NAN;
	if (check_reponse_in_recbuf(&response_at_port_usbmodem[0],10))
	{
		at_port = GSM_USB_MODEM;
	}
	else if (check_reponse_in_recbuf(&response_at_port_uart1[0],7))
	{
		at_port = GSM_UART1;
	}
	else if (check_reponse_in_recbuf(&response_at_port_cmux1[0],7))
	{
		at_port = GSM_CMUX1;
	}
	else if (check_reponse_in_recbuf(&response_at_port_cmux2[0],7))
	{
		at_port = GSM_CMUX2;
	}
	else if (check_reponse_in_recbuf(&response_at_port_cmux3[0],7))
	{
		at_port = GSM_CMUX3;
	}
	else if (check_reponse_in_recbuf(&response_at_port_cmux4[0],7))
	{
		at_port = GSM_CMUX4;
	}
	else
	{

	}
	return at_port;
}
/**
 * @brief 
 * This function to get the specified socket status.
 * NOTE : don't change the sequence of the parameters inside the function . 4 8 9
 */
void GSM_get_socket_status(void)
{
	/**+QISTATE: **/
	if (check_reponse_in_recbuf(&response_sockets_state[3],7))
	{
		gsm.trails =10;
		/**get connected ID**/
		uint8_t id = GSM_get_state_connected_ID();
		/**get service type **/
		sockets_status[id].service_type = GSM_get_state_service_type();
		/**get IP**/
		GSM_get_state_IP(&sockets_status[id].remote_IP[0]);
		/**get remote port **/
		sockets_status[id].remote_port = GSM_get_number_32Bit();
		/**get local port **/
		sockets_status[id].local_port = GSM_get_number_32Bit();
		/**get socket state **/
		sockets_status[id].socket_state = GSM_get_number_32Bit();
		/**get context ID**/
		sockets_status[id].context_ID = GSM_get_number_32Bit();
		/**get server ID**/
		sockets_status[id].server_ID = GSM_get_number_32Bit();
		/**get access mode**/
		sockets_status[id].access_mode = GSM_get_number_32Bit();
		/**get AT port **/
		sockets_status[id].AT_port = GSM_get_at_port();
		TCP_state = GSM_HANDLE_RECEIVED_MESSAGES;
	}
}

/**
 * @brief 
 * 
 */
void GSM_reset_receive_buffer(void)
{
	command_buff_tail =0;
	command_buff_head =1; 
	for (uint8_t i= 0; i<COMMAND_BUFF_SIZE;i++)
	{
		command_buff[i]=0;
	}
	/*memset(command_buff,0,BUFF_MAX_SIZE);*/
}
/**
 * @brief 
 * 
 */
void GSM_send_tampers(void)
{		
		GSM_disconnection_timeout();
	/*if there is any tampers send it */
	if ( tamper_alarm_flags.fullaccess > 0 )
	{
		/**check if there is any change in tampers state or not **/
		if (tamper_alarm_flags.fullaccess != previous_status_tampers.fullaccess)
		{
			if (strlen(remote_server)>1)
			{
				if (0 < gsm.remote_exist)
				{
					/**check if the server in communication with other due to timeout **/
					if (0 == gsm.disconnect_timeout)
					{
						if(GSM_SERVICE_TYPE_TCP_LISTENER == sockets_status[0].service_type)
						{
							if ( 1 == gsm.tampers_switch )
							{
								TCP_state =GSM_CLOSE_SERVER;			
								gsm.required_connection = GSM_SERVICE_TYPE_TCP;	
								execution_thd = 49;
								gsm.tampers_switch = 0;
							}
						}
						/*//if(GSM_SOCKET_STATE_OPENING==sockets_status[0].socket_state)*/
						if (GSM_SERVICE_TYPE_TCP == gsm.required_connection)
						{
							GSM_request_status();
						}
						
						if ((GSM_SOCKET_STATE_CONNECTED==sockets_status[0].socket_state) &&( GSM_SERVICE_TYPE_TCP == sockets_status[0].service_type))
						{
							GSM_reset_receive_buffer();
							/** TODO:: configure it in display module **/
							uint32_t meter_ID_display=0;
							uint8_t pack_buf[10]={	(uint8_t)meter_ID_display,
													(uint8_t)(meter_ID_display>>8),
													(uint8_t)(meter_ID_display>>16),
													(uint8_t)(meter_ID_display>>24),
													(uint8_t)tamper_alarm_flags.fullaccess,
													(uint8_t)(tamper_alarm_flags.fullaccess>>8),
													(uint8_t)(tamper_alarm_flags.fullaccess>>16),
													(uint8_t)(tamper_alarm_flags.fullaccess>>24),'\r','\n'};
							/**send meter ID**/
							GSM_send_data("AT+QISEND=0\r\n",13);
							delay_us(5000);
							GSM_send_data(pack_buf,10);
							delay_ms(5);	
							uint8_t ack=0x1A;	/**this is the acknowldge to complete transmittion**/
							GSM_send_data(&ack, 1);
							gsm.send_tamper =1;
							TCP_state =GSM_SEND;
							execution_thd =49;
							gsm.required_connection = GSM_SERVICE_TYPE_TCP;
							sockets_status[0].service_type =GSM_SERVICE_TYPE_IDLE;
							sockets_status[0].context_ID=0;
						}	
					}
				}
			}	
		}
	}
	
	 
}

/**
 * @brief 
 * this function to check the timeout after last connection as a server.   
 */

void GSM_disconnection_timeout(void)
{	
	gsm.disconnect_timeout --;
	/**after disconnect_timeout goes to zero it truncted to be 65535 **/
	if(gsm.disconnect_timeout > DISCONNECTION_TIME) 
	{
		gsm.disconnect_timeout = 0 ; 
	}
}
/**
 * @brief 
 * 
 */
void GSM_request_status(void)
{	
	if(GSM_HANDLE_RECEIVED_MESSAGES == TCP_state)
	{ 
		if(0 == gsm.disconnect_timeout)
		{
			TCP_state = GSM_SOCKET_SERVICE_STATUS;
		}
	}
}
/**
 * @brief 
 * 
 */
void GSM_send_Ack(void)
{
	if(1 == gsm.send_tamper)
	{
		uint8_t ack=0x1A;	/**this is the acknowldge to complete transmittion**/
		/*//GSM_send_data(&ack, 1);*/
		gsm.send_tamper = 0;
	}
}
/**
 * @brief 
 * 
 */
void GSM_enable_remote_server_check(void)
{
	gsm.remote_exist = 10 ; 
}
#pragma endregion UTILITIES_FUNCTION

