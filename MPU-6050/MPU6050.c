#include "MPU6050.h"

MPU6050_HandleTypedef MPU_HTD;
FunctionalState funcST;

static HAL_StatusTypeDef MPU6050_initGyro(uint8_t Gyro_FS);
static HAL_StatusTypeDef MPU6050_initAccel(uint8_t Accel_FS);
static HAL_StatusTypeDef MPU6050_setBit(uint8_t RegAddr,uint8_t *Data);
static HAL_StatusTypeDef MPU6050_clearBit(uint8_t RegAddr,uint8_t *Data);


void MPU6050_init(MPU6050_HandleTypedef MPU_conf)
{
	MPU_HTD.i2c=MPU_conf.i2c;
	MPU_HTD.FM=MPU_conf.FM;
	
	
	//config
	MPU6050_Sleep(DISABLE);
	MPU6050_FIFOMode(MPU_HTD.FM);
	MPU6050_initGyro(MPU6050_GYRO_FS_500);
	MPU6050_initAccel(MPU6050_Accel_FS_2g);
}
HAL_StatusTypeDef MPU6050_enableTempture(FunctionalState EN)
{
	if (EN==DISABLE)
		return MPU6050_setBit(MPU6050_REG_PWR_MGMT_1,(uint8_t*)MPU6050_PWR_MGMT_1_TEMP_DIS);
	else
		return MPU6050_clearBit(MPU6050_REG_PWR_MGMT_1,(uint8_t*)MPU6050_PWR_MGMT_1_TEMP_DIS);
}
HAL_StatusTypeDef MPU6050_standbyGyro(FunctionalState EN)
{
	if (EN==ENABLE)
		return MPU6050_setBit(MPU6050_REG_PWR_MGMT_1,(uint8_t*)MPU6050_PWR_MGMT_1_GYRO_STANDBY);
	else
		return MPU6050_clearBit(MPU6050_REG_PWR_MGMT_1,(uint8_t*)MPU6050_PWR_MGMT_1_GYRO_STANDBY);
}
HAL_StatusTypeDef MPU6050_Sleep(FunctionalState EN)
{
	if (EN==ENABLE)
		return MPU6050_setBit(MPU6050_REG_PWR_MGMT_1,(uint8_t*)MPU6050_PWR_MGMT_1_SLEEP);
	else
		return MPU6050_clearBit(MPU6050_REG_PWR_MGMT_1,(uint8_t*)MPU6050_PWR_MGMT_1_SLEEP);
}
HAL_StatusTypeDef MPU6050_RESET(void)
{
	return MPU6050_setBit(MPU6050_REG_PWR_MGMT_1,(uint8_t*)MPU6050_PWR_MGMT_1_RST);
}
HAL_StatusTypeDef MPU6050_FIFOMode(FIFOMode FM)
{
	if (FM==FULL)
		return MPU6050_setBit(MPU6050_REG_CONFIG,(uint8_t *)MPU6050_CONFIG_FIFOMODE);
	else
		return MPU6050_clearBit(MPU6050_REG_CONFIG,(uint8_t *) MPU6050_CONFIG_FIFOMODE);
}
HAL_StatusTypeDef MPU6050_writeReg(uint8_t RegAddr,uint8_t *Data,uint16_t Size)
{
	uint8_t DataTransmit[2];
	DataTransmit[0]=RegAddr;
	
	for (int k=0;k<Size;k++)
		DataTransmit[k+1]=Data[k];
	
	if (HAL_I2C_Master_Transmit(&hi2c1,MPU6050_ADDRESS,DataTransmit,Size+1,10)!=HAL_OK)
		return HAL_ERROR;
	
	return HAL_OK;
}
HAL_StatusTypeDef MPU6050_readReg(uint8_t RegAddr,uint8_t *Data,uint16_t Size)
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



//////private function//////////////
static HAL_StatusTypeDef MPU6050_initConfig();
static HAL_StatusTypeDef MPU6050_initGyro(uint8_t Gyro_FS)
{
	return MPU6050_setBit(MPU6050_REG_GYRO_CONFIG,&Gyro_FS);
}

static HAL_StatusTypeDef MPU6050_initAccel(uint8_t Accel_FS)
{
	return MPU6050_setBit(MPU6050_REG_GYRO_CONFIG,&Accel_FS);
}
static HAL_StatusTypeDef MPU6050_setBit(uint8_t RegAddr,uint8_t *Data)
{
	uint8_t Data_reg=0x00;
	if(MPU6050_readReg(RegAddr,&Data_reg,1)!=HAL_OK)
		return HAL_ERROR;
	
	SET_BIT(Data_reg,*Data);
	
	if (MPU6050_writeReg(RegAddr,&Data_reg,1)!=HAL_OK)
		return HAL_ERROR;
	
	return HAL_OK;
	
}
static HAL_StatusTypeDef MPU6050_clearBit(uint8_t RegAddr,uint8_t *Data)
{
	uint8_t Data_reg=0x00;
	if(MPU6050_readReg(RegAddr,&Data_reg,1)!=HAL_OK)
		return HAL_ERROR;
	
	CLEAR_BIT(Data_reg,*Data);
	if (MPU6050_writeReg(RegAddr,&Data_reg,1)!=HAL_OK)
		return HAL_ERROR;
	
	return HAL_OK;
	
}