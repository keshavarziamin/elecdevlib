/*
 * RemoteASK.cpp
 *
 *  Created on: Jan 1, 2022
 *      Author: Amin-PC
 */

#include "RemoteASK.h"
#include <algorithm>
#include "stdio.h"
#include "string.h"

typedef enum __receiveStage{
	RECEIVE_CHECK_PREAMBLE,
	RECEIVE_GET_DATA,
	RECEIVE_EXTRACT_DATA
}ReceiveStage;



RemoteASK::RemoteASK(){
	portRx.gpio=nullptr;
	portTx.gpio=nullptr;
	htim=nullptr;
}
__Status RemoteASK::init(const RemoteASK_GPIO &gpioRx){
	if (!gpioRx.gpio)
		return __Status::ERROR;

	this->ferquancy=ferquancy;
	this->portRx=gpioRx;

	return __Status::OK;
}
__Status RemoteASK::init(const RemoteASK_GPIO &gpioRx,const RemoteASK_GPIO &gpioTx){
	if (!gpioRx.gpio || !gpioTx.gpio)
		return __Status::ERROR;


	this->portRx=gpioRx;
	this->portTx=gpioTx;

	return __Status::OK;
}

__Status RemoteASK::setConfigTimer(TIM_HandleTypeDef *htim,const uint32_t &clk,const RemoteASK_FERQUANCY &ferquancy){

	//IM auto-reload register

	if (!htim)
		return __Status::ERROR1;

	this->ferquancy=ferquancy;
	this->htim=htim;
	decltype(this->htim->Instance->PSC) PSC=clk/1000000; //calculate PSC
	this->htim->Instance->PSC=(PSC*this->ferquancy)-1;
	this->htim->Instance->CNT=0;//reset counter register

	decltype(this->htim->Instance->ARR) ARP =MAX_RELOAD;
	this->htim->Instance->ARR=ARP;

	if (HAL_TIM_Base_Start(this->htim)) //if timer return error
		return __Status::ERROR2;

	return __Status::OK;
}


__Status RemoteASK::isPressed(){

	uint32_t data=0;

	if (!flag.decoding)
		return __Status::ERROR1;

	data=decode();

	if(!data)
		return __Status::ERROR2;

	if(data!=buffer.data)
		buffer.data=data;

	return __Status::OK;
}


/*
 * @brief: decode the received frame to data including ID and key
 * @return : the decoded data, zero shows the error
 * */
uint32_t RemoteASK::decode(void){
	std::vector<uint32_t> dataVector;
	uint32_t extractedData=0;
	uint8_t *frame_ptr=std::begin(buffer.RX);
	__Status retStatus=__Status::OK;


	fixToleranceRxBuffer();

	//get all data in buffer
	//TODO: change coding
	while(1){

		retStatus=checkPreamble(&frame_ptr);
		if(retStatus!=__Status::OK)
			break;
		extractedData=extractData(frame_ptr);
		if(extractedData)
			dataVector.push_back(extractedData);
	}


	memset(buffer.RX,0,LENGTH_RXBUFFER); //clear buffer

	auto acptData= acceptData(dataVector); //return the decoded data
	activateReceiving();

	return acptData;
}

/*
 * @brief: check preamble of data frame
 * @return: status of find (__Status) */
__Status RemoteASK::checkPreamble(uint8_t **pptr){

	static auto *start_ptr=std::begin(buffer.RX);

	*pptr=std::find(start_ptr,std::end(buffer.RX),PREAMBLE_HIGH);

	//check pointer
	if(!*pptr || (*pptr+LENGTH_FRAME) >= std::end(buffer.RX)){
		start_ptr=std::begin(buffer.RX);
		return __Status::ERROR1;
	}

	if (*(*pptr-1)!=DATA_LOW)//and the previous value is equal to data_low
		return __Status::ERROR2;

	(*pptr)++; //return the start pointer of frame
	start_ptr=*pptr;
	return __Status::OK;
}
/*@brief: fix tolerance of frame
 * */
void RemoteASK::fixToleranceRxBuffer(){

	for(int i=0; i<LENGTH_RXBUFFER ;i++){
		if(buffer.RX[i]<(DATA_HIGH))
			buffer.RX[i]=DATA_LOW;
		else if(buffer.RX[i]<PREAMBLE_HIGH-(TOLERANCE*5))
			buffer.RX[i]=DATA_HIGH;
		else if(buffer.RX[i]>=PREAMBLE_HIGH-(TOLERANCE*5) && buffer.RX[i]<PREAMBLE_HIGH+(TOLERANCE*5))
			buffer.RX[i]=PREAMBLE_HIGH;
		else
			buffer.RX[i]=(uint8_t)NULL;
	}
}
/*
 * @brief: extract ID and key
 * frame_ptr: the start pointer of frame
 * return: the data including specific ID and number of key
 * */
uint32_t RemoteASK::extractData(uint8_t *frame_ptr){
	auto length=NUMBER_OF_BITs;
	auto a=*frame_ptr;
	auto b=a;
	uint32_t data=0;
	while(length--){
		a=*frame_ptr++;
		b=*frame_ptr++;
		data<<=1;
		if(!a ||!b) // if the value be zero
			return 0;
		data|=(a==3 && b==1) ? 0x01UL:0x00UL;
	}
	return data;
}

uint32_t RemoteASK::acceptData(std::vector<uint32_t> vector){
	uint8_t max=0,count=0;
	uint32_t acceptedData=0;

	std::sort(vector.begin(), vector.end());

	//detect distinct and accepted data
	for(auto itr=vector.begin();itr<=vector.end();itr++){
		count=std::count(vector.begin(),vector.end(),*itr);
		//get the accepted data
		if(count>max){
			max=count;
			acceptedData=*itr;
		}
		itr+=count; //ignore the same value
	}

	return acceptedData;
}
/*
 * @brief: receive frame of data according to timer and counter
 *
 */
void RemoteASK::receive(void){
	static uint32_t index=0;
	static uint32_t timer=uwTick,delay=400;

	if(flag.decoding)//if system is decoding data, remote must not be get data
		return;
	//check timer
	if((timer+delay)<uwTick)
		index=0; //reset index if the frame has not be got after delay
	timer=uwTick;//reset timer

	buffer.RX[index++]=GET_COUNTER; //get value of counter after interrupt
	RESET_COUNTER; //set zero
	if (index>LENGTH_RXBUFFER){
		index=0;
		deactivateReceiving(); //active flag of decoding data
	}
	return;

}
RemoteKey  RemoteASK::getReceivedKey (void){
	return (RemoteKey) (buffer.data & 0x0000000F);
}
uint32_t RemoteASK::getReceivedID(void){
	return (buffer.data & 0x00FFFFF0);
}

void RemoteASK::activateReceiving(){
	flag.decoding=false;
}
void RemoteASK::deactivateReceiving(){
	flag.decoding=true;
}
