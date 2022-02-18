/*
 * uart_handler.h
 *
 *  Created on: Dec 11, 2021
 *      Author: Amin-PC
 */

#ifndef APPLICATIONS_UART_HANDLER_H_
#define APPLICATIONS_UART_HANDLER_H_

#include "status.h"
#include "usart.h"
#include <string>

#define __GET_CNDTR() uart->hdmarx->Instance->CNDTR
#define SIZE_OF_BUFFER 1024

namespace Application{

struct __UART_BUFFER{
	uint32_t NDTR;
	uint8_t RX[SIZE_OF_BUFFER];
	uint8_t *rxptr;
};




class UartReceiver{

public:
	UartReceiver(){};
	~UartReceiver(){};

	__Status init(UART_HandleTypeDef *);
	int getData(uint8_t *RxData,uint16_t length,uint16_t timeout);
	__Status sendData(uint8_t *,uint16_t );
	__Status sendData(std::string const &str);
protected:

private:
	UART_HandleTypeDef *uart;
	__UART_BUFFER rcvBuffer;


	void recordLastPointer();
	uint16_t  getReceivedCounter(uint16_t ,uint16_t );
	__Status startReceiving();
	__Status restartPort();

};
}
#endif /* SYSTEM_UART_HANDLER_H_ */
