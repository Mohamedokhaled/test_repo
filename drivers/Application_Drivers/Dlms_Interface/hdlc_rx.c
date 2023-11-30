/*
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>*/
#include "BaseTime.h"

#include "hdlc_rx.h"

#include "RS485.h"

extern basetime_t time_tick;
uint32_t timeout_flag;


hdlc_proc_context_t hdlc_Optical_proc_context;
hdlc_proc_context_t hdlc_RS485_proc_context;
hdlc_proc_context_t hdlc_GSM_4G_proc_context;

static hdlc_config_t shadow_config;


void hdlc_rx_proc_context_reset(hdlc_proc_context_t *context) {
	context->state = RX_IDLE;
	context->frame_index = 0U;
	context->frame_length = 0U;
	context->last_timestamp_ms = 0U;

	time_tick.T1_rx_ms=0U;
	memset(context->buf, 0U, sizeof(context->buf));
}

void hdlc_rx_proc_init(hdlc_proc_context_t *context) {
	hdlc_rx_proc_context_reset(context);
	context->shadow_config_state = SHADOW_INVALID;
	context->is_frame_ready = 0;
}

void hdlc_rx_proc_config(hdlc_proc_context_t *context,
	uint32_t inactivity_time_out, uint32_t inter_octet_time_out) {
	if(context->state == RX_IDLE) {
		context->config.inactivity_time_out = inactivity_time_out;
		context->config.inter_octet_time_out = inter_octet_time_out;
	} else {
		shadow_config.inactivity_time_out = inactivity_time_out;
		shadow_config.inter_octet_time_out = inter_octet_time_out;
		context->shadow_config_state = SHADOW_RECONFIG;
	}
}

static void hdlc_rx_proc_reconfig(hdlc_proc_context_t *context) {
	if(context->shadow_config_state == SHADOW_RECONFIG) {
		context->config.inactivity_time_out = shadow_config.inactivity_time_out;   /*if no frame recieved and frame is not complete*/
		context->config.inter_octet_time_out = shadow_config.inter_octet_time_out; 
		context->shadow_config_state = SHADOW_INVALID;
	}
}

void hdlc_rx_set_frame_ready(hdlc_proc_context_t* context) {
	context->is_frame_ready = 1U;
}

uint8_t hdlc_rx_get_frame_ready(hdlc_id_t id) {
	uint32_t ret;
	switch(id)
	{
		case HDLC_OPTICAL:
			ret = hdlc_Optical_proc_context.is_frame_ready;
		break;
		case HDLC_RS485: 
			ret = hdlc_RS485_proc_context.is_frame_ready;
		break;
		case HDLC_GSM_4G:
			ret =hdlc_GSM_4G_proc_context.is_frame_ready;
		
		break;
	}
	return (ret);
}

void hdlc_rx_set_frame_consumed(hdlc_proc_context_t* context) {
	context->is_frame_ready = 0;
	context->frame_length = 0 ;
	context->frame_index = 0 ; 
	context->state =RX_IDLE;
	/*asamir*/
	memset(context->buf, 0U, sizeof(context->buf));
}

uint32_t sys_timestamp_ms(void) {
	return (time_tick.T1_rx_ms/1000);
}

uint32_t sys_time_elapsed_ms(uint32_t start_timestamp) {
	if(time_tick.T1_rx_ms > start_timestamp){
		timeout_flag = 1;
	}
	else{
		timeout_flag = 0;
	}
	return (timeout_flag);
}

static void print_frame(uint8_t *frame, uint32_t len, uint32_t break_config) {
	uint32_t break_line = 0;
	printf("Frame length = %u: \n", len);
    for(int i = 0; i < len; i++) {
    	printf("0x%x ", frame[i]);
    	break_line ++;
    	if(break_line == break_config) {
    		printf("\n");
    		break_line = 0;
    	}
    }
    printf("\n");
}

int8_t hdlc_received_connect_ID = -1;
void hdlc_Tx_proc(uint8_t* frame, uint32_t size, hdlc_id_t id)
 {
	 switch(id)
	 {
		 case HDLC_OPTICAL:
				/*  usart_serial_write_packet(UART0,frame,size); */
		 break;
		 case HDLC_RS485:
		 		rs485_send(frame,size);
				/**TODO:: Handle RS485 flow control **/
				/* RS485_write_message(frame, size);
				delay_ms(1);
				ioport_set_pin_level(RS_DRIVER_ENABLE,false); */
		 break;
		 
		 case  HDLC_GSM_4G:
		 
			{
				/**TODO:: Handle 4G module context **/
				switch(hdlc_received_connect_ID)
				{
			
					case 1:
					case 2:
					case 3:
					case 4:
					case 5:
					case 6:
					case 7:
					case 8:
					case 9:
					case 10:
					case 11:
					{
							/* TCP_send_4G(frame,size,hdlc_received_connect_ID);
							incomming_counter=0; */
					
					}
					break;
					default:
					break;
				}
			}
		 break;
	 }

	
}

void hdlc_rx_proc_4G(uint8_t c,hdlc_id_t id, uint8_t connect_ID)
{
	hdlc_rx_proc(c , id);
	hdlc_received_connect_ID = connect_ID;
}
void hdlc_rx_proc (uint8_t c,hdlc_id_t id)
{
	hdlc_proc_context_t *context ;
	switch(id)
	{
		case HDLC_OPTICAL:
		context=&hdlc_Optical_proc_context;
		break;
		
		case HDLC_RS485:
		context=&hdlc_RS485_proc_context;
		break;
		case  HDLC_GSM_4G:
		context=&hdlc_GSM_4G_proc_context;
		
		break;
	}
	
	if(context->startTimeOut == 1){
		if(sys_time_elapsed_ms(100)) {
			hdlc_rx_proc_context_reset(context);
			context->startTimeOut=0;
		}
	}


	switch (context->state)
	{
	   case RX_IDLE:
		if(c == 0x7E) {
			context->buf[context->frame_index++] = c;
			context->state = RX_STARTED;
		} else {
			/*hdlc_rx_proc_reconfig(context);*/
			hdlc_rx_proc_context_reset(context);

		}
		context->startTimeOut = 1;
		time_tick.T1_rx_ms=0;

		break;
	   case RX_STARTED:
		if(context->frame_index == 3U) {
			context->frame_length = ((context->buf[1] & 0x07) << 8U) | context->buf[2];
		}
		context->buf[context->frame_index++] = c;
		if(context->frame_index == context->frame_length + 1) {
			context->state = RX_WAIT_END;
		}
		break;
	   case RX_WAIT_END:
		if(c != 0x7E) {
			/* discard the frame */
			hdlc_rx_proc_context_reset(context);
		} else {
			context->buf[context->frame_index++] = c;
			/* Consider frame start and end flag "7E" */
			context->frame_length += 2 ;
			hdlc_rx_set_frame_ready(context);
			context->startTimeOut = 0;
		}
			context->state = RX_IDLE;
		break;
	}
}

void hdlc_rx_timeout_handle(void)
{

	if(hdlc_GSM_4G_proc_context.startTimeOut == 1){
		if(sys_time_elapsed_ms(100)) {
			hdlc_rx_proc_context_reset(&hdlc_GSM_4G_proc_context);
			hdlc_GSM_4G_proc_context.startTimeOut=0;
		}
	}
	if(hdlc_Optical_proc_context.startTimeOut == 1){
			if(sys_time_elapsed_ms(100)) {
				hdlc_rx_proc_context_reset(&hdlc_Optical_proc_context);
				hdlc_Optical_proc_context.startTimeOut=0;
			}
		}
	if(hdlc_RS485_proc_context.startTimeOut == 1){
			if(sys_time_elapsed_ms(100)) {
				hdlc_rx_proc_context_reset(&hdlc_RS485_proc_context);
				hdlc_RS485_proc_context.startTimeOut=0;
			}
		}

}

