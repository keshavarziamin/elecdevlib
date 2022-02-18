/*
 * RemoteASK.h
 *
 *  Created on: Jan 1, 2022
 *      Author: Amin-PC
 */




#ifndef DEVICE_DRIVERS_REMOTEASK_H_
#define DEVICE_DRIVERS_REMOTEASK_H_

#include "stm32f1xx_hal.h"
#include "gpio.h"
#include "tim.h"
#include <vector>
#include "status.h"

#define MAX_DELAY 1000
#define TOLERANCE 1
#define NUMBER_OF_BITs 24
#define LENGTH_FRAME NUMBER_OF_BITs*2
#define MAX_RELOAD 250
#define PREAMBLE_HIGH 31
#define DATA_HIGH 3
#define DATA_LOW 1
#define LENGTH_RXBUFFER 150
#define GET_COUNTER (uint32_t)__HAL_TIM_GET_COUNTER(this->htim)
#define RESET_COUNTER __HAL_TIM_SET_COUNTER(this->htim,0)//reset counter;

typedef enum class __keyRemote{
	KEY_A=0x00000001,
	KEY_B=0x00000002,
	KEY_C=0x00000004,
	KEY_D=0x00000008,
	KEY_NULL=0
}RemoteKey;

typedef enum __ferquancy{
	REMOTE_ASK_433_MHz=320,
	REMOTE_ASK_315_MHz=2
}RemoteASK_FERQUANCY;




typedef struct __Remoteflag{
	bool decoding=false;
}RemoteFlag;

typedef struct __buffer{
	uint8_t RX[LENGTH_RXBUFFER]={0};
	uint32_t data=0;
}RemoteBuffer;

typedef struct __gpio{
	GPIO_TypeDef *gpio=nullptr;
	uint32_t pin;
}RemoteASK_GPIO;

class RemoteASK {
public:
	RemoteASK();
	virtual ~RemoteASK(){};

	__Status init(const RemoteASK_GPIO &gpioRx);
	__Status init(const RemoteASK_GPIO &gpioRx,const RemoteASK_GPIO &gpioTx);
	__Status isPressed();
	__Status setConfigTimer(TIM_HandleTypeDef *,const uint32_t &,const RemoteASK_FERQUANCY &);
	__Status add(void);
	__Status remove(void);
	__Status transmit(void);
	void receive(void);
	void activateReceiving();
	void deactivateReceiving();
	uint32_t getReceivedID();
	RemoteKey getReceivedKey ();

	uint32_t acceptData(std::vector<uint32_t>);

	RemoteASK_GPIO portRx;
	RemoteASK_GPIO portTx;
	RemoteKey pressedKey=RemoteKey::KEY_NULL;

private:
	uint32_t 		decode();
	void 			fixToleranceRxBuffer();
	__Status 	checkPreamble(uint8_t **pptr);
	uint32_t 		extractData(uint8_t *);

	__Status 	checkID(uint32_t);


	uint32_t clk;
	uint32_t usDelay;
	RemoteASK_FERQUANCY ferquancy;
	TIM_HandleTypeDef *htim=nullptr;
	RemoteBuffer buffer;
	RemoteFlag flag;

};

#endif /* DEVICE_DRIVERS_REMOTEASK_H_ */
