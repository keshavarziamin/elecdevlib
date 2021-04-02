#include "ICM20948.h"


#define READ_MODE(x) 	(x)|=0x80	//enable read mode and set 1 the MSB bit
#define WRITE_MODE(x) (x)&=0x7F //enable write mode  and set 0 the MSB bit



ICM_HandleTypedef* ICM;

static void chipSelect_enable(ICM_HandleTypedef* pICM);
static void chipSelect_disable(ICM_HandleTypedef* pICM);

HAL_StatusTypeDef ICM_init(ICM_HandleTypedef* pICM)
{
	uint8_t PWR_MGMT[2]={0x00,0x00};  //
	chipSelect_disable(pICM);
	
	//config power managment registers
	if (pICM->Init.GYRO==ENABLE)
		PWR_MGMT[1]&=~ICM_PWR_MGMT_2_GYRO_DISABLE;
	else
		PWR_MGMT[1]|=ICM_PWR_MGMT_2_GYRO_DISABLE;
	
	if (pICM->Init.ACCEL==ENABLE)
		PWR_MGMT[1]&=~ICM_PWR_MGMT_2_ACCEL_DISABLE;
	else
		PWR_MGMT[1]|=ICM_PWR_MGMT_2_ACCEL_DISABLE;
	
	if (pICM->Init.TEMP==ENABLE)
		PWR_MGMT[0]&=~ICM_PWR_MGMT_1_TEMP_DIS;
	else
		PWR_MGMT[0]|=ICM_PWR_MGMT_1_TEMP_DIS;
	
	//select clock source 
	PWR_MGMT[0]|=(pICM->Init.clockSource)&0x07;
	//wake up device
	PWR_MGMT[0]&=~ICM_PWR_MGMT_1_SLEEP;
	
	
	
	
	
	//write registers
	if (ICM_selectBank(pICM,ICM_USER_BANK_0)!=HAL_OK)
		return HAL_ERROR;
	if(ICM_writeRegister(pICM,ICM_ADDR_PWR_MGMT_1,PWR_MGMT,2)!=HAL_OK)
		return HAL_ERROR;
	
	//read bank 2
	uint8_t gyro_config[2]={0x11,0x07},accel_config=0x15;
	if (ICM_selectBank(pICM,ICM_USER_BANK_2)!=HAL_OK)
		return HAL_ERROR;
	
	
	if(ICM_writeRegister(pICM,ICM_ADDR_GYRO_CONFIG_1,gyro_config,2)!=HAL_OK)
		return HAL_ERROR;
	if(ICM_writeRegister(pICM,ICM_ADDR_ACCEL_CONFIG_1,&accel_config,1)!=HAL_OK)
		return HAL_ERROR;
	
	
	uint8_t RB2[24];
	ICM_readRegister(pICM,0x00,RB2,24);
	
	
	return HAL_OK;
}



//high level function


HAL_StatusTypeDef ICM_getGyro(ICM_HandleTypedef* pICM)
{
	uint8_t const lengthOfRxData=6; //gyro x,y,z
	uint8_t RxData[lengthOfRxData];
	
	if (ICM_selectBank(pICM,ICM_USER_BANK_0)!=HAL_OK)
		return HAL_ERROR;
	if(ICM_readRegister(pICM,ICM_ADDR_GYRO_XOUT_H,RxData,lengthOfRxData)!=HAL_OK)
		return HAL_ERROR;

	
	pICM->Register.GYRO.X.half=(int16_t)((RxData[0]<<8)|RxData[1]);
	pICM->Register.GYRO.Y.half=(int16_t)((RxData[2]<<8)|RxData[3]);
	pICM->Register.GYRO.Z.half=(int16_t)((RxData[4]<<8)|RxData[5]);

	
	return HAL_OK;
	
}
HAL_StatusTypeDef ICM_getAccel(ICM_HandleTypedef* pICM)
{
	uint8_t const lengthOfRxData=6; //gyro x,y,z
	uint8_t RxData[lengthOfRxData];
	
	if (ICM_selectBank(pICM,ICM_USER_BANK_0)!=HAL_OK)
		return HAL_ERROR;
	if(ICM_readRegister(pICM,ICM_ADDR_ACCEL_XOUT_H,RxData,lengthOfRxData)!=HAL_OK)
		return HAL_ERROR;
	

	
	pICM->Register.ACCEL.X.half=(int16_t)((RxData[0]<<8)|RxData[1]);
	pICM->Register.ACCEL.Y.half=(int16_t)((RxData[2]<<8)|RxData[3]);
	pICM->Register.ACCEL.Z.half=(int16_t)((RxData[4]<<8)|RxData[5]);
	
	return HAL_OK;
	
}

HAL_StatusTypeDef ICM_calculateAngle(ICM_HandleTypedef* pICM)
{
	// read these variables
	float gx, gy, gz;
	float ax, ay, az;
	float mx, my, mz;
	float theta,phi,psi;
	
	static float pitch=0.0,roll=0.0;
	//get data
	gx = (float)(pICM->Register.GYRO.X.half)/131.0;
	gy = (float)(pICM->Register.GYRO.Y.half)/131.0;
	gz = (float)(pICM->Register.GYRO.Z.half)/131.0;
	//printf("gx=%f gy=%f gz=%f\r\n",gx,gy,gz);
	
	
	ax=(float)(pICM->Register.ACCEL.X.half)/4095.0;
	ay=(float)(pICM->Register.ACCEL.Y.half)/4095.0;
	az=(float)(pICM->Register.ACCEL.Z.half)/4095.0;
	//printf("ax=%f ay=%f az=%f\r\n",ax,ay,az);
	
	mx=0.0;
	my=0.0;
	mz=0.0;
	
	//debug
	//printf("gx=%f gy=%f gz=%f\r",gx,gy,gz);
	//printf("ax=%f ay=%f az=%f\r",ax,ay,az);
	
	gx=gx*M_PI/180;
	gy=gy*M_PI/180;
	gz=gz*M_PI/180;
	
	
	
	// use the following function to fuse sensor data and compute quaternion
	//MadgwickAHRSupdate(gx, gy, gz, ax, ay, az, mx, my, mz);
	MahonyAHRSupdate(gx, gy, gz, ax, ay, az, mx, my, mz);
	//MahonyAHRSupdate(gy, gx, gz, ay, ax, az, mx, my, mz);
	//MahonyAHRSupdate(gx, gy, 0, ax, ay, 0, mx, my, mz);
	
	// use the following function to convert quaternions to Euler angles
	quaternion2euler(&phi,&theta,&psi);
	//q2e(&phi,&theta,&psi);
	//printf("phi1=%f\t theta1=%f\t psi1=%f\r\n",phi,theta,psi);
	//avraging
	pICM->Register.ANGLE.phi+=phi;
	pICM->Register.ANGLE.phi/=2;
	pICM->Register.ANGLE.theta+=theta;
	pICM->Register.ANGLE.theta/=2;
	pICM->Register.ANGLE.psi+=psi;
	pICM->Register.ANGLE.psi/=2;
	
	//printf("phi1=%f\t theta1=%f\t psi1=%f\r",phi,theta,psi);
	
	
}


HAL_StatusTypeDef ICM_selectBank(ICM_HandleTypedef* pICM,uint8_t bankNumber)
{
	HAL_StatusTypeDef status;
	bankNumber=bankNumber<<4;
	status=ICM_writeRegister(pICM,ICM_ADDR_REG_BANK_SEL,&bankNumber,1);
	
	return status;
	
}


HAL_StatusTypeDef ICM_checkBank(ICM_HandleTypedef* pICM,uint8_t *bankNumber)
{
	HAL_StatusTypeDef status;
	status=ICM_readRegister(pICM,ICM_ADDR_REG_BANK_SEL,bankNumber,1);
	*bankNumber=*bankNumber>>4;
	return status;
	
}




//low level functions
HAL_StatusTypeDef ICM_readRegister(ICM_HandleTypedef* pICM,uint8_t address, uint8_t *PRXData, uint8_t length)
{
	HAL_StatusTypeDef status;
	uint8_t lengthOfReceivedBuffer=length+1;
	uint8_t *receivedbuffer=malloc(lengthOfReceivedBuffer*sizeof(uint8_t));
	uint8_t timeOut=TIMEOUT;
	
	READ_MODE(address);
	//read data
	chipSelect_enable(pICM);
	HAL_Delay(1);

	while(timeOut--){
		status=HAL_SPI_TransmitReceive(pICM->Config.spi,&address,receivedbuffer,lengthOfReceivedBuffer,40);
		if (status==HAL_OK){
			clearErrorBit(ERR_ICM_HW);
			break;
		}
		HAL_Delay(100);
	}
	
	if (status!=HAL_OK)
		setErrorBit(ERR_ICM_HW);
	
	chipSelect_disable(pICM);
	//return buffer
	memcpy(PRXData,&receivedbuffer[1],length);
	//clear and free buffer
	memset(receivedbuffer,0,lengthOfReceivedBuffer);
	free(receivedbuffer);
	
	return status;
}


HAL_StatusTypeDef ICM_writeRegister(ICM_HandleTypedef* pICM,uint8_t address, uint8_t *PTXData,uint8_t length)
{
	HAL_StatusTypeDef status;
	WRITE_MODE(address);
	uint8_t lenghtOfTrasmitBuffer=length+1;
	uint8_t *transmitBuffer=malloc(lenghtOfTrasmitBuffer*sizeof(uint8_t));
	uint8_t timeOut=TIMEOUT;
	//combining address and data
	transmitBuffer[0]=address;
	memcpy(&transmitBuffer[1],PTXData,length);
	//send data
	chipSelect_enable(pICM);
	HAL_Delay(1);
	status=HAL_ERROR;
	while(timeOut--){
		status=HAL_SPI_Transmit(pICM->Config.spi,transmitBuffer,lenghtOfTrasmitBuffer,40);
		if (status==HAL_OK){
			clearErrorBit(ERR_ICM_HW);
			break;
		}
		HAL_Delay(100);
	}
	if (status!=HAL_OK)
		setErrorBit(ERR_ICM_HW);
	
	chipSelect_disable(pICM);
	//clear and free buffer
	memset(transmitBuffer,0,lenghtOfTrasmitBuffer);
	free(transmitBuffer);
	
	return status;
}





static void chipSelect_enable(ICM_HandleTypedef* pICM)
{
	HAL_GPIO_WritePin(pICM->Config.CS_GPIO,pICM->Config.CS_Pin,GPIO_PIN_RESET);
}
static void chipSelect_disable(ICM_HandleTypedef* pICM)
{
	HAL_GPIO_WritePin(pICM->Config.CS_GPIO,pICM->Config.CS_Pin,GPIO_PIN_SET);
}
