#include "MPU6050.h"

MPU6050_HandleTypedef MPU_HTD;
FunctionalState funcST;
float accel_SSF;  //Sensitivity Scale Factor


static HAL_StatusTypeDef MPU6050_initGyro(uint8_t Gyro_FS);
static HAL_StatusTypeDef MPU6050_initAccel(uint8_t Accel_FS);
static HAL_StatusTypeDef MPU6050_setBit(uint8_t RegAddr,uint8_t *Data);
static HAL_StatusTypeDef MPU6050_clearBit(uint8_t RegAddr,uint8_t *Data);


void MPU6050_init(MPU6050_HandleTypedef MPU_conf)
{
	MPU_HTD.i2c=MPU_conf.i2c;
	MPU_HTD.FM  =MPU_conf.FM;
	MPU_HTD.sleepState    =MPU_conf.sleepState;
	MPU_HTD.GYRO_FS_scale =MPU_conf.GYRO_FS_scale;
	MPU_HTD.ACCEL_FS_scale=MPU_conf.ACCEL_FS_scale;
	MPU_HTD.temptureState =MPU_conf.temptureState;
	//config
	MPU6050_Sleep(MPU_HTD.sleepState);
	MPU6050_FIFOMode	(MPU_HTD.FM);
	MPU6050_initGyro	(MPU_HTD.GYRO_FS_scale);
	MPU6050_initAccel	(MPU_HTD.ACCEL_FS_scale);
	MPU6050_enableTempture(MPU_HTD.temptureState);
	
	/*
			AFS_SEL=0 16,384 	LSB/g
			AFS_SEL=1 8,192 	LSB/g
			AFS_SEL=2 4,096 	LSB/g
			AFS_SEL=3 2,048 	LSB/g
	*/
	switch(MPU_HTD.ACCEL_FS_scale)
	{
		case MPU6050_ACCEL_FS_2g	:
			accel_SSF=16384;
			break;
		case MPU6050_ACCEL_FS_4g	:
			accel_SSF=8192;
			break;
		case MPU6050_ACCEL_FS_8g  :
			accel_SSF=4096;
			break;
		case MPU6050_ACCEL_FS_16g :
			accel_SSF=2048;
			break;	
	}
}




HAL_StatusTypeDef MPU6050_getAccel(float *AccelData)
{
	/*
	@breif	: get value of accelerometer (x,y,z)
	@param	: AccelData is the output parameter that has to be 16 bits  and 3 index
	@return : the status of I2C
	NOTE: Reset value: 0x00 (if sensor disabled)	
	*/
	uint8_t Data_re[6];
	// MPU6050_REG_ACCEL_XOUT_H is the start address of gyroscope registers 
	if (MPU6050_readReg(MPU6050_REG_ACCEL_XOUT_H,Data_re,6)!=HAL_OK)
		return HAL_ERROR;
	
	//combine two bytes to a 16 bits(half word) 
	AccelData[0]=(float)(Data_re[0]<<8|Data_re[1])/accel_SSF;  //X value
	AccelData[1]=(float)(Data_re[2]<<8|Data_re[3])/accel_SSF;	//Y Value
	AccelData[2]=(float)(Data_re[4]<<8|Data_re[5])/accel_SSF;	//Z Value
	
	
	
	return HAL_OK;
}
HAL_StatusTypeDef MPU6050_getGyro(uint16_t *GyroData)
{
	/*
	@breif: get value of gyroscope (x,y,z)
	@param: GyroData is the output parameter that has to be 16 bits  and 3 index
	@return : the status of I2C 
	NOTE: Reset value: 0x00 (if sensor disabled)
	*/
	uint8_t Data_re[6];
	// MPU6050_REG_GYRO_XOUT_H is the start address of gyroscope registers 
	if (MPU6050_readReg(MPU6050_REG_GYRO_XOUT_H,Data_re,6)!=HAL_OK)
		return HAL_ERROR;
	
	//combine two bytes to a 16 bits(half word) 
	GyroData[0]=(Data_re[0]<<8|Data_re[1]);  //X value
	GyroData[1]=(Data_re[2]<<8|Data_re[3]);		//Y Value
	GyroData[2]=(Data_re[4]<<8|Data_re[5]);	//Z Value
	
	return HAL_OK;
}
HAL_StatusTypeDef MPU6050_getTempture(float *TEMP_degC)
{
	/*
	To convert the output of the temperature sensor to degrees
	C please use the formula below:
	TEMP_degC = ((TEMP_OUT –RoomTemp_Offset)/Temp_Sensitivity)+ 21degC
	*/
	uint8_t temp[2];
	uint16_t temp_16=0x0000;
	
	if (MPU6050_readReg(MPU6050_REG_TEMP_OUT_H,temp,2)!=HAL_OK)
		return HAL_ERROR;
	
	temp_16=(temp[1]<<8|temp[0]);
	
	//convert to degree C
	//*TEMP_degC=((float)(temp_16-MPU6050_TEMPTURE_OFFSET)/(float)MPU6050_TEMPTURE_SENSITIVITY)+21;
	*TEMP_degC=(float)temp_16;
	return HAL_OK;
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
	
	if (HAL_I2C_Master_Transmit(&MPU_HTD.i2c,MPU6050_ADDRESS,DataTransmit,Size+1,10)!=HAL_OK)
		return HAL_ERROR;
	
	return HAL_OK;
}
HAL_StatusTypeDef MPU6050_readReg(uint8_t RegAddr,uint8_t *Data,uint16_t Size)
{
	uint8_t DataReceive[8];
	for (int k=0;k<Size;k++)
		DataReceive[k]=0;
	
 	DataReceive[0]=RegAddr;
	
	//request to read register
	if (HAL_I2C_Master_Transmit(&MPU_HTD.i2c,MPU6050_ADDRESS,DataReceive,1,10)!=HAL_OK)
		return HAL_ERROR;
	//read regiser
	if (HAL_I2C_Master_Receive(&MPU_HTD.i2c,MPU6050_ADDRESS,&DataReceive[1],Size,10)!=HAL_OK)
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