
#include "utilities.h"
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include "usart.h"

#define  BL_DEBUG_MSG_EN
#define D_UART   &huart6

/* prints formatted string to console over UART */
 void printmsg(char *format,...)
 {
#ifdef BL_DEBUG_MSG_EN`
	char str[80];

	/*Extract the the argument list using VA apis */
	va_list args;
	va_start(args, format);
	vsprintf(str, format,args);
	HAL_UART_Transmit(D_UART,(uint8_t *)str, strlen(str),HAL_MAX_DELAY);
	va_end(args);
#endif
 }
