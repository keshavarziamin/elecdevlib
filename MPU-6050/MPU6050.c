#include "MPU6050.h"

MPU6050_HandleTypedef MPU_conf;
void MPU6050_init(I2C_HandleTypeDef *hi2c)
{
	MPU_conf.i2c=hi2c;
	uint8_t value=0x21;
	MPU6050_writeData(MPU6050_REG_PWR_MGMT_1,&value,1);
}

HAL_StatusTypeDef MPU6050_writeData(uint8_t RegAddr,uint8_t *Data,uint16_t Size)
{
	uint8_t DataTransmit[2];
	DataTransmit[0]=RegAddr;
	
	for (int k=0;k<Size;k++)
		DataTransmit[k+1]=Data[k];
	
	if (HAL_I2C_Master_Transmit(&hi2c1,MPU6050_ADDRESS,DataTransmit,Size+1,10)!=HAL_OK)
		return HAL_ERROR;
	
	return HAL_OK;
}
HAL_StatusTypeDef MPU6050_readData(uint8_t RegAddr,uint8_t *Data,uint16_t Size)
{
	uint8_t DataReceive[5];
	for (int k=0;k<Size;k++)
		DataReceive[k]=0;
	
 	DataReceive[0]=RegAddr;
	
	//request to read register
	if (HAL_I2C_Master_Transmit(&hi2c1,MPU6050_ADDRESS,DataReceive,1,10)!=HAL_OK)
		return HAL_ERROR;
	//read regiser
	if (HAL_I2C_Master_Receive(&hi2c1,MPU6050_ADDRESS,&DataReceive[1],4,10)!=HAL_OK)
		return HAL_ERROR;
	
	for (int k=0;k<Size;k++)
		Data[k]=DataReceive[k+1];
	
	return HAL_OK;
}