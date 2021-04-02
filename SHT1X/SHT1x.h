#ifndef SHT1x_H
#define SHT1x_H



#include "stm32f4xx_hal.h"
#include "gpio.h"
#include "usart.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
//#include "logger.h"

#define DEBUG 0
#define PRINT(X,Y)   HAL_UART_Transmit(&huart3,X,Y,100);


//humidity conversion coefficients
//low resolution
#define C1_LOW_V3  			-2.0468
#define C2_LOW_V3				0.5872
#define C3_LOW_V3				-4.0845e-4

#define C1_LOW_V4 			-4.0
#define C2_LOW_V4				0.0405
#define C3_LOW_V4				-2.8e-6

//high resolution
#define C1_HIGH_V3    -2.0468
#define C2_HIGH_V3		0.0367
#define C3_HIGH_V3		-1.5955e-6

#define C1_HIGH_V4		-4.0
#define C2_HIGH_V4		0.0405
#define C3_HIGH_V4		-2.8000e-6



//Temperature conversion coefficients
/*
Temperature conversion coefficients are realted to VCC
5V 		--> VDD=5V
4V		-->	VDD=4V
3.5V	--> VDD=3.5V
3V		--> VDD=3V
2.5V	--> VDD=2.5V

C --> centigrade
*/

#define D1_C_5V   -40.1
#define D1_C_4V		-39.8
#define D1_C_35V	-39.7
#define D1_C_3V		-39.6
#define D1_C_25V	-39.4

#define T1_LOW_RESOLUTION			0.01
#define T2_LOW_RESOLUTION			0.00008
#define T1_HIGH_RESOLUTION		0.01
#define T2_HIGH_RESOLUTION		0.00008

#define D2_C_HIGH_RESOLUTION 	0.01 
#define D2_C_LOW_RESOLUTION		0.04



//address of status register bits
#define STATUS_REGISTER_RESOLUTION   	0x01
#define STATUS_REGISTER_OTP						0x02
#define STATUS_REGISTER_HEATER				0x04

//address of register
#define SHT_REGISTER_STATUS						0x06
#define SHT_REGISTER_HUMIDITY					0x04
#define SHT_REGISTER_TEMPETURE				0x02


//times
#define TIME_PERIOD				4 						//period time of clock (value * 80 * time of for loop)
#define TIME_PERIOD_2			TIME_PERIOD/2 //micro second

#define TIME_MEASUREMENT_320  320000 				//wait to measure value of tempture or humidity 
#define TIME_MEASUREMENT_240  320000 				//wait to measure value of tempture or humidity 
#define TIME_MEASUREMENT_80  320000 				//wait to measure value of tempture or humidity


#define SET_DEVICE_ADDRESS(X)   	(X)=(X)&0x1F; //address of device is 0b000xxxxx;
#define SET_READ_MODE(X) 					(X)=(X)|0x1;  //set the lsb bit to 1 to read register 0bxxxxxxx1
#define SET_WRITE_MODE(X)					(X)=(X)&0xFE; //set the lsb bit to 0 to write register 0bxxxxxxx0



#define SCKPIN_RESET			HAL_GPIO_WritePin(SHT_X->GPIO_SCK.Port,SHT_X->GPIO_SCK.Pin,GPIO_PIN_RESET)
#define SCKPIN_SET				HAL_GPIO_WritePin(SHT_X->GPIO_SCK.Port,SHT_X->GPIO_SCK.Pin,GPIO_PIN_SET)
#define SDAPIN_RESET			HAL_GPIO_WritePin(SHT_X->GPIO_SDA.Port,SHT_X->GPIO_SDA.Pin,GPIO_PIN_RESET)
#define SDAPIN_SET				HAL_GPIO_WritePin(SHT_X->GPIO_SDA.Port,SHT_X->GPIO_SDA.Pin,GPIO_PIN_SET)


#define SCKPIN_RESET_DELAY			Delay(TIME_PERIOD_2);SCKPIN_RESET;Delay(TIME_PERIOD_2)
#define SCKPIN_SET_DELAY				Delay(TIME_PERIOD_2);SCKPIN_SET;Delay(TIME_PERIOD_2)	
#define SDAPIN_RESE_DELAY				Delay(TIME_PERIOD_2);SDAPIN_RESET;Delay(TIME_PERIOD_2)
#define SDAPIN_SET_DELAY				Delay(TIME_PERIOD_2);SDAPIN_SETDelay(TIME_PERIOD_2)	


#define SDAPIN_READ				HAL_GPIO_ReadPin(SHT_X->GPIO_SDA.Port,SHT_X->GPIO_SDA.Pin)


typedef enum
{
	SHT1x_OK=0x00,
	SHT1x_ERROR,
	SHT1x_TIMEOUT
	
}	SHT1x_StatusTypedef;

typedef struct 
{
	float  C1,C2,C3;  //humidity conversion coefficients
	float 	TCC1,TCC2,T1,T2;//Temperature conversion coefficients
}SHT1x_coefficientsTypedef;

typedef struct
{
	FunctionalState highResolution;
	FunctionalState heater;
	FunctionalState reloadOTP;
	SHT1x_coefficientsTypedef Coffi; //humidity and tempture conversion coefficients
}SHT1x_configTypedef;

typedef struct 
{
	GPIO_TypeDef *Port;
	uint16_t     Pin;
}SHT1x_GPIOTypedef;

typedef struct
{
	float Temp;    //read only
	float humidity; //read only
	uint8_t Status;    //read and write 
}	SHT1x_RegisterTypedef;


//coefficients15
typedef struct 
{
	SHT1x_GPIOTypedef GPIO_SCK;
	SHT1x_GPIOTypedef GPIO_SDA;
	SHT1x_RegisterTypedef reg;
	SHT1x_configTypedef   config;
	uint8_t errorCode;
	SHT1x_StatusTypedef Status;   
}SHT1x_HandelTypedef;


//public functions

SHT1x_StatusTypedef SHT1x_init(SHT1x_HandelTypedef *PSHT1x);
SHT1x_StatusTypedef SHT1x_writeStatusReg(uint8_t write_Data);
SHT1x_StatusTypedef SHT1x_readStatusReg(uint8_t *pData);
SHT1x_StatusTypedef SHT1x_GetTemptureHumidity(float *valueOfHumidity,float *valueOfTempture);

SHT1x_StatusTypedef SHT1x_WriteRegister(uint8_t regAddr,uint8_t *pData);
SHT1x_StatusTypedef SHT1x_readRegister(uint8_t regAddr,uint8_t *pData, uint8_t length);



//static function




#endif

