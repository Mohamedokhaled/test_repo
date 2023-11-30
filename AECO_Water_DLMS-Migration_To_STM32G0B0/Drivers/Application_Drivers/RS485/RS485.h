#ifndef __RS485_H__
#define __RS485_H__
#include <stdint.h>

#define BUFFER_SIZE   255
typedef struct 
{
  uint8_t head;
  uint8_t tail;
  uint8_t buffer[BUFFER_SIZE];
  uint8_t ready;
}buffer_t;


extern volatile buffer_t rs485;
void rs485_Init(void);

void rs485_send(uint8_t * data,uint32_t len);
void rs485_receive_callback (uint8_t data) ; 



#endif
