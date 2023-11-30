
#ifndef _HDLC_RX_
#define _HDLC_RX_


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define HDLC_MAX_FRAME_LEN	1500U
#define HDLC_MIN_FRAME_LEN	11U

#define CRITICAL_SECTION_START
#define CRITICAL_SECTION_END

 typedef enum {
	RX_IDLE	= 0U,
	RX_STARTED	= 1U,
	RX_WAIT_END	= 2U,
	RX_END		= 3U,

	RX_MAX_VALUE	= 65535U
} hdlc_rx_state_t;

typedef enum 
{
	HDLC_OPTICAL,
	HDLC_RS485,
	HDLC_GSM_4G
}hdlc_id_t;

typedef enum {
	SHADOW_INVALID 	= 0U,
	SHADOW_RECONFIG	= 1U,

	SHADOW_MAX_VALUE	= INT32_MAX
} hdlc_shadow_config_state_t;

typedef struct {
	uint32_t		inactivity_time_out;
	uint32_t		inter_octet_time_out;
} hdlc_config_t;

typedef struct {
	hdlc_rx_state_t 	state;
	uint32_t		frame_index;
	uint32_t 		frame_length;
	uint32_t		last_timestamp_ms;
	uint8_t startTimeOut;
	uint8_t		buf[HDLC_MAX_FRAME_LEN];
	hdlc_config_t		config;
	hdlc_shadow_config_state_t shadow_config_state;
	uint32_t			is_frame_ready;
} hdlc_proc_context_t;



 
extern hdlc_proc_context_t hdlc_Optical_proc_context;
extern hdlc_proc_context_t hdlc_RS485_proc_context;
extern hdlc_proc_context_t hdlc_GSM_4G_proc_context;

/*static hdlc_config_t	    shadow_config;*/

void hdlc_rx_proc_context_reset(hdlc_proc_context_t *context) ;

void hdlc_rx_proc_init(hdlc_proc_context_t *context) ;

void hdlc_rx_proc_config(hdlc_proc_context_t *context,
	uint32_t inactivity_time_out, uint32_t inter_octet_time_out) ;

void hdlc_rx_set_frame_ready(hdlc_proc_context_t* context) ;
uint8_t hdlc_rx_get_frame_ready(hdlc_id_t id);
void hdlc_rx_set_frame_consumed(hdlc_proc_context_t* context) ;

uint32_t sys_timestamp_ms(void) ;

uint32_t sys_time_elapsed_ms(uint32_t start_timestamp) ;

void hdlc_rx_proc (uint8_t c,hdlc_id_t id);
void hdlc_rx_proc_4G(uint8_t c,hdlc_id_t id, uint8_t connect_ID);
void hdlc_Tx_proc(uint8_t* frame, uint32_t size,hdlc_id_t id);
void hdlc_rx_timeout_handle(void);
#endif
