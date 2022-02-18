

#include "string.h"
#include "UartReceiver.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "stdio.h"

namespace Application{

__Status UartReceiver::init(UART_HandleTypeDef *uart){

	if (!uart)
		return __Status::ERROR1;

	this->uart=uart;

	if (startReceiving()!=__Status::OK)
		return __Status::ERROR2;

	recordLastPointer();

	printf("uart_rx with DMA -> activate\r\n");
	return __Status::OK;
}

int UartReceiver::getData(uint8_t *RxData,uint16_t length,uint16_t timeout ){



	if(restartPort()!=__Status::OK)
		return (int)__Status::ERROR;


	uint16_t counter=0,extraLen=0;
	auto RXbfr_size=uart->RxXferSize;
	auto Rxbfr_pBase=uart->pRxBuffPtr;
	auto Rxbfr_pEnd=Rxbfr_pBase+RXbfr_size;


	counter=getReceivedCounter(length,timeout);//get the counter of NDTR
	if (!counter)
		return counter;


	//if counter + last pointer of Rx buffer is bigger than the last pinter
	//the first step system have to copy the data from  rx pointer to last pointer
	//the next step the system have to copy the remained data
	if((counter+rcvBuffer.rxptr)>Rxbfr_pEnd){
	extraLen=(counter+rcvBuffer.rxptr)-Rxbfr_pEnd;
	counter=Rxbfr_pEnd-rcvBuffer.rxptr;
	}

	memcpy(RxData,rcvBuffer.rxptr,counter);

	if (extraLen)
		memcpy(&RxData[counter],Rxbfr_pBase,extraLen);

	recordLastPointer();

	return (counter+extraLen);

}
__Status UartReceiver::sendData(uint8_t *data,uint16_t len){

	if (!data)
		return __Status::ERROR1;

	if (HAL_UART_Transmit(uart,data,len,100)!=HAL_OK)
		return __Status::ERROR2;
	return __Status::OK;
}

__Status UartReceiver::sendData(std::string const &str){

	if (!str.length())
		return __Status::ERROR1;

	const char *ptr=str.c_str();
	if (HAL_UART_Transmit(uart,(uint8_t *)ptr,str.length(),100)!=HAL_OK)
		return __Status::ERROR2;
	return __Status::OK;
}
uint16_t  UartReceiver::getReceivedCounter(uint16_t length,uint16_t timeout){

	uint16_t counter=0;
	auto currentNDTR=__GET_CNDTR();


	/*checking the length of received data*/
	while(timeout--){

		if(currentNDTR>rcvBuffer.NDTR)
			counter=uart->RxXferSize+rcvBuffer.NDTR-currentNDTR;
		else
			counter=rcvBuffer.NDTR-currentNDTR; //difference of NDTR

		if (counter>=length)
			break;

		vTaskDelay(1);//wait 1 ms

		currentNDTR=__GET_CNDTR(); //get the new CNDTR
	}


	return counter;
}


void UartReceiver::recordLastPointer(){
	auto Rxbfr_pBase=uart->pRxBuffPtr;
	auto RXbfr_size=uart->RxXferSize;
	//record the last pointer and NDTR
	rcvBuffer.NDTR=__GET_CNDTR(); //record the new CNDTR
	rcvBuffer.rxptr=Rxbfr_pBase+(RXbfr_size-rcvBuffer.NDTR);//record the last pointer of rx rcvBuffer that get data
}

__Status UartReceiver::startReceiving(){
	if(HAL_UART_Receive_DMA(this->uart, rcvBuffer.RX, SIZE_OF_BUFFER))
			return __Status::ERROR;
	return __Status::OK;
}

__Status UartReceiver::restartPort(){

	__Status status;
	uint8_t revrscntr=5; //received Counter
	if (uart->gState==HAL_UART_STATE_READY)
		return __Status::OK; //return OK if uart is ready

	//if uart is not ready, it must be reseted
	while(revrscntr--){
		HAL_UART_DMAStop(uart);//stop DMA
		memset(rcvBuffer.RX,0,SIZE_OF_BUFFER); //clear buffer
		status=startReceiving();
		if(status==__Status::OK)
			break;
		vTaskDelay(10);
	}
	return status;
}
}
