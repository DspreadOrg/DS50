#ifndef _API_UART_H
#define _API_UART_H
#include "api_types.h"

enum {
	API_COM0=0,
	API_COM1=1,
	API_COM2=2,
	API_COM3=3,
};


enum
{
	API_HAL_UART_NO_PARITY,   ///< No parity check
	API_HAL_UART_ODD_PARITY,  ///< Parity check is odd
	API_HAL_UART_EVEN_PARITY, ///< Parity check is even
};
 
int OsUartInit(int comport, int baud, int parity, int recvtimeout, void* recv_cb);


 
int OsUartSend(int comport, unsigned char* buff, unsigned int len);
 
int OsUartClose(int comport);






#endif
