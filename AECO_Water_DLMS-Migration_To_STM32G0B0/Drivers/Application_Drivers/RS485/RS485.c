#include "RS485.h"
#include "gpio.h"
#include "stm32g0xx_it.h"
#include  "usart.h"
#include "hdlc_rx.h"

volatile buffer_t rs485={
.head=0,
.tail=1,
.ready=0,
};


void rs485_Init(void)
{
  
   MX_USART3_UART_Init();
	/*enable interrupt for receive line */
  __HAL_UART_ENABLE_IT(&huart3, UART_IT_RXNE);
}

void rs485_send(uint8_t * data, uint32_t len)
{
    /**TODO:: activate HW flow control in transmission mode   **/

    HAL_UART_Transmit(&huart3,data,len,150);
    /**TODO:: activate HW flow control in reception mode   **/
}


void rs485_receive_callback (uint8_t data)
{
  hdlc_rx_proc(data,HDLC_RS485);
}
