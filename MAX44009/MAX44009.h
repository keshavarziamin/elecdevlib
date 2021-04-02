#ifndef  	__MAX44009_H
#define 	__MAX44009_H

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>


#define MAX_DEVICE_ADDR_PULLUP 0x02
#define MAX_DEVICE_ADDR_PULLDOWN 0x00
#define MAX_DEVICE_ADDRESS 0x94






#define MAX_REG_ADDR_INTERRUPT_STATUS			0x00			
#define MAX_REG_ADDR_INTERRUPT_ENABLE			0x01
#define MAX_REG_ADDR_CONFIGURATION				0x02	
#define MAX_REG_ADDR_LUX_HIGH_BYTE				0x03	
#define MAX_REG_ADDR_LUX_LOW_BYTE					0x04	
#define MAX_REG_ADDR_UPPER_THRESHOLD			0x05		
#define MAX_REG_ADDR_LOWER_THRESHOLD			0x06	
#define MAX_REG_ADDR_THRESHOLD TIMER			0x07


//interrup register bits
#define MAX_REG_INT_ENABLE								0x01		
//confuration register bits
#define MAX_REG_CONFIGURATION_CONT				0x80
#define MAX_REG_CONFIGURATION_MANUAL			0x40
#define MAX_REG_CONFIGURATION_CDR					0x08

#define MAX_INTGRATION_TIME_800			0x00		 //This is a preferred mode for boosting low-light sensitivity.
#define MAX_INTGRATION_TIME_400			0x01		
#define MAX_INTGRATION_TIME_200			0x02		
#define MAX_INTGRATION_TIME_100			0x03			//This is a preferred mode for high-brightness applications	
#define MAX_INTGRATION_TIME_50			0x04			//Manual mode only.
#define MAX_INTGRATION_TIME_25			0x05		  //Manual mode only.
#define MAX_INTGRATION_TIME_12R5		0x06	    //Manual mode only.
#define MAX_INTGRATION_TIME_6R5			0x07		  //Manual mode only.


typedef enum{
	PULL_UP=2,
	PULL_DOWN=0
}MAX_DEVICE_ADDR_FUNCTIONAL;

typedef enum{
		EN=0,
		DIS=1
}MAX_DEVICE_STATE_FUNCTIONAL;


typedef struct
{
	uint8_t interruptStatus;
	uint8_t interruptEnable;
	uint8_t Configuration;
	uint8_t LUXHigh;
	uint8_t LUXLow;
	uint8_t threshUpper;
	uint8_t threshLower;
	uint8_t	threshTimer;
}__attribute((packed))  MAX_REGISTER;


typedef struct 
{
	I2C_HandleTypeDef *hi2c;
	GPIO_TypeDef  *INTPort;
	uint32_t INTPin;
	MAX_DEVICE_ADDR_FUNCTIONAL deviceADDR_Pin;
}__attribute((packed))  MAX_HARDWARE;

typedef struct 
{
	MAX_DEVICE_STATE_FUNCTIONAL Interrupt;
	MAX_DEVICE_STATE_FUNCTIONAL ContinuousMode;
	MAX_DEVICE_STATE_FUNCTIONAL ManualConfiguration;
	MAX_DEVICE_STATE_FUNCTIONAL CurrentDivision;
	
	uint8_t IntegrationTime;
}__attribute((packed))  MAX_CONFIGURATION;

typedef struct
{
	uint8_t 						err;
	MAX_REGISTER 				Reg;
	MAX_HARDWARE 				HW;
	MAX_CONFIGURATION   Config;
}__attribute((packed))  MAX_HANDLETYPEDEF;


int MAX_init(MAX_HANDLETYPEDEF *MAX);
int MAX_getLUX(MAX_HANDLETYPEDEF *MAX,float *LUX);
int MAX_setIntegrationTime(MAX_HANDLETYPEDEF *MAX,uint8_t time);
int MAX_setBit(MAX_HANDLETYPEDEF *MAX,uint8_t regAddr,uint8_t bits);
int MAX_clearBit(MAX_HANDLETYPEDEF *MAX,uint8_t regAddr,uint8_t bits);
int MAX_enableContinuous(MAX_HANDLETYPEDEF *MAX);
int MAX_disableContinuous(MAX_HANDLETYPEDEF *MAX);
int MAX_enableManual(MAX_HANDLETYPEDEF *MAX);
int MAX_disableManual(MAX_HANDLETYPEDEF *MAX);
int MAX_enableCDR(MAX_HANDLETYPEDEF *MAX);
int MAX_disableCDR(MAX_HANDLETYPEDEF *MAX);
int MAX_setUpperThresshod(MAX_HANDLETYPEDEF *MAX,float upperThreshLUX);
int MAX_setLowerThresshod(MAX_HANDLETYPEDEF *MAX,float lowerThreshLUX);
int MAX_readRegister(MAX_HANDLETYPEDEF *MAX,uint8_t regAddr,uint8_t *RxData);
int MAX_writeRegister(MAX_HANDLETYPEDEF *MAX,uint8_t regAddr,uint8_t TxData);

float MAX_calculateLUX(uint8_t exp,uint8_t mantissa);
uint8_t MAX_calculateMantissa(float value);
#endif
