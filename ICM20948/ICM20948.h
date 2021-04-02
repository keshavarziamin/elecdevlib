#ifndef ICM20948_H
#define ICM20948_H


#include "stm32f4xx_hal.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
//#include "LOOKUPTABLE.h"
#include "MahonyAHRS.h"
#include "MadgwickAHRS.h"
#include "alert.h"


#define TIMEOUT					5			//5 * 100ms;  
/*REG_BANK_SEL*/
#define ICM_USER_BANK_0 0x00
#define ICM_USER_BANK_1	0x01
#define ICM_USER_BANK_2	0x02
#define ICM_USER_BANK_3	0x03


/*PWR_MGMT_1*/
#define ICM_PWR_MGMT_1_DEVICE_RESET				0x80
#define ICM_PWR_MGMT_1_SLEEP							0x40
#define ICM_PWR_MGMT_1_LP_EN							0x20
#define ICM_PWR_MGMT_1_TEMP_DIS						0x08
/*
Code: Clock Source
0: Internal 20 MHz oscillator
1-5: Auto selects the best available clock source – PLL if ready, else use the Internal oscillator
6: Internal 20 MHz oscillator
7: Stops the clock and keeps timing generator in reset
NOTE: CLKSEL[2:0] should be set to 1~5 to achieve full gyroscope performance.
*/
#define ICM_PWR_MGMT_1_CLKSEL_INTERNAL		0x00
#define ICM_PWR_MGMT_1_CLKSEL_AUTO				0x02
#define ICM_PWR_MGMT_1_CLKSEL_STOP				0x07 

/*PWR_MGMT_2*/
/*
Only the following values are applicable:
111 – Accelerometer (all axes) disabled.
000 – Accelerometer (all axes) on.
*/
#define ICM_PWR_MGMT_2_ACCEL_DISABLE		0x38   
/*
Only the following values are applicable:
111 – Gyroscope (all axes) disabled.
000 – Gyroscope (all axes) on.
*/
#define ICM_PWR_MGMT_2_GYRO_DISABLE 		0x07




//register address
#define ICM_ADDR_REG_BANK_SEL 0x7F

/*USER BANK 0 REGISTER MAP*/
#define ICM_ADDR_PWR_MGMT_1		0x06
#define ICM_ADDR_PWR_MGMT_2		0x07

#define ICM_ADDR_GYRO_XOUT_H  0x33
#define ICM_ADDR_GYRO_XOUT_L	0x34
#define ICM_ADDR_GYRO_YOUT_H	0x35
#define ICM_ADDR_GYRO_YOUT_L	0x36
#define ICM_ADDR_GYRO_ZOUT_H	0x37
#define ICM_ADDR_GYRO_ZOUT_L	0x38

#define ICM_ADDR_ACCEL_XOUT_H 0x2D
#define ICM_ADDR_ACCEL_XOUT_L	0x2E
#define ICM_ADDR_ACCEL_YOUT_H	0x2F
#define ICM_ADDR_ACCEL_YOUT_L	0x30
#define ICM_ADDR_ACCEL_ZOUT_H	0x31
#define ICM_ADDR_ACCEL_ZOUT_L	0x32


/*USER BANK 2 REGISTER MAP*/
#define ICM_ADDR_GYRO_CONFIG_1 0x01 
#define ICM_ADDR_ACCEL_CONFIG_1 0x14 

//bank 0
union twoByte
{
	uint8_t byte[2];
	int16_t half;
};

typedef struct
{
	union twoByte X;
	union twoByte Y;
	union twoByte Z;
}ICM_XYZTypedef;	
typedef struct
{
	float phi;
	float theta;
	float psi;
}ICM_AngleTypedef;	
typedef struct 
{
	FunctionalState GYRO;
	FunctionalState TEMP;
	FunctionalState ACCEL;
	/*
	clock source:
	ICM_PWR_MGMT_1_CLKSEL_INTERNAL
	ICM_PWR_MGMT_1_CLKSEL_AUTO		
	ICM_PWR_MGMT_1_CLKSEL_STOP		
	*/
	uint8_t clockSource; 
}ICM_InitTypedef;

typedef struct
{
	ICM_XYZTypedef GYRO;
	ICM_XYZTypedef ACCEL;
	ICM_XYZTypedef MAGN;
	ICM_XYZTypedef TEMP;
	ICM_AngleTypedef ANGLE;
	
}ICM_Registertypedef;

typedef struct 
{
	SPI_HandleTypeDef* spi;
	GPIO_TypeDef* CS_GPIO;
	uint16_t CS_Pin;
}ICM_ConfigTypedef;




typedef struct 
{
	ICM_ConfigTypedef Config;
	ICM_Registertypedef Register;
	ICM_InitTypedef			Init;
}ICM_HandleTypedef;
//top level functions
HAL_StatusTypeDef ICM_init(ICM_HandleTypedef* pICM);

/**
*@brief: check number of register bank
*@param:	bankNamber: select number of bank from "REG_BANK_SEL"
											ICM_USER_BANK_0
											ICM_USER_BANK_1
											ICM_USER_BANK_2
											ICM_USER_BANK_3
*@retval: return 	HAL_OK: 
									HAL_ERROR: 
								
*/
HAL_StatusTypeDef ICM_selectBank(ICM_HandleTypedef* pICM,uint8_t bankNumber);

/**
*@brief: check number of register bank
*@retval: return 	HAL_OK: the data is valid;
									HAL_ERROR: the data is not valid;
									bankNamber: return 0,1,2,3 
*/
HAL_StatusTypeDef ICM_checkBank(ICM_HandleTypedef* pICM,uint8_t *bankNumber);


//gyroscop function
/**
	*@brief: get data of gyroscop
	*@retval: return 	HAL_OK: the data is valid;
									HAL_ERROR: the data is not valid;
									gyroAxis: return axis of gyro that is ICM_XYZTypedef
	*@Note: to get data, gyro has to be enable 
*/
HAL_StatusTypeDef ICM_getGyro(ICM_HandleTypedef* pICM);

//Accel function
/**
	*@brief: get data of accel
	*@retval: return 	HAL_OK: the data is valid;
									HAL_ERROR: the data is not valid;
									accelAxis: return axis of accel that is ICM_XYZTypedef
	*@Note: to get data, gyro has to be enable 
*/
HAL_StatusTypeDef ICM_getAccel(ICM_HandleTypedef* pICM);

HAL_StatusTypeDef ICM_calculateAngle(ICM_HandleTypedef* pICM);
void ICM_saveMaximumAngle(ICM_HandleTypedef* pICM);

//low level functions
HAL_StatusTypeDef ICM_readRegister(ICM_HandleTypedef* pICM,uint8_t address, uint8_t *PRXData, uint8_t length);
HAL_StatusTypeDef ICM_writeRegister(ICM_HandleTypedef* pICM,uint8_t address, uint8_t *PTXData,uint8_t length);


#endif



