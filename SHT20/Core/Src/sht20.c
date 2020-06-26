/*
 * sht20.c
 *
 *  Created on: Jun 12, 2020
 *      Author: amin
 */


#include "sht20.h"
#include "string.h"
#include "stdlib.h"



int sht20_init(SHT20_HandelTypedef *pSHT){
	uint8_t userReg=0;
	uint8_t recvuserReg;

	if (HAL_I2C_IsDeviceReady(pSHT->hardware.pI2C, SHT20_I2C_ADDRESS, 5, 100)!=HAL_OK)
		return -1;
	HAL_Delay(100);

	#ifdef PRINT_DEBUG
		printf("device (%02x) is ready. \r\n",SHT20_I2C_ADDRESS);
	#endif


	if (pSHT->configuration.OTPReload==ENABLE)
		CLEAR_BIT( userReg,SHT20_OTP_RELOAD );
	else
		SET_BIT(userReg,SHT20_OTP_RELOAD );

	if (pSHT->configuration.onChipHeater==ENABLE)
		SET_BIT(userReg,SHT20_ON_CHIP_HEATER);
	else
		CLEAR_BIT(userReg,SHT20_ON_CHIP_HEATER);

	SET_BIT(userReg,pSHT->configuration.meaturementResulation);



	if (sht20_writeUserRegister(&pSHT->hardware.pI2C, &userReg)!=0)
		return -2 ;

	if (sht20_readUserRegister(&pSHT->hardware.pI2C, &recvuserReg)!=0)
		return -3;

	if ((userReg&recvuserReg)!=userReg)
		return -4;

	#ifdef PRINT_DEBUG
		printf("the value of userReg is %x\r\n",userReg);
	#endif

	pSHT->reg.userRegister=recvuserReg;
	return 0;
}




int sht20_getTemperature(SHT20_HandelTypedef *pSHT,int8_t *pRecvTemp,HoldMasterMode mode ){

	uint8_t RxBuffer[SHT20_LENGTH_OF_DATA];


	switch (mode){
	case HOLD_MASTER:
		if (sht20_readData_hlodMaster(pSHT->hardware.pI2C,(uint8_t) SHT20_CMD_T_MSURMT, RxBuffer)!=0)
			return -1;
		break;
	case NO_HOLD_MASTER:
		if (sht20_readData_noHlodMaster(pSHT->hardware.pI2C,(uint8_t) SHT20_CMD_T_MSURMT, &RxBuffer)!=0)
			return -2;
		break;

	default:
		return -3;
	}

	//check CRC
	if (SHT20_checkCRC(RxBuffer,SHT20_LENGTH_OF_DATA)!=0)
		return -4;

	*pRecvTemp=SHT20_conversionTemperature(&RxBuffer);

	#ifdef PRINT_DEBUG
		printf_hex("temperature data is ", RxBuffer, SHT20_LENGTH_OF_DATA);
		printf("temp is %d\r\n",*pRecvTemp);
	#endif
	return 0;

}

int sht20_getHumidity(SHT20_HandelTypedef *pSHT,int8_t *pRecvHmty,HoldMasterMode mode ){

	uint8_t RxBuffer[SHT20_LENGTH_OF_DATA];
	uint8_t CRC8;

	switch (mode){
	case HOLD_MASTER:
		if (sht20_readData_hlodMaster(pSHT->hardware.pI2C,(uint8_t) SHT20_CMD_RH_MSURMT, RxBuffer)!=0)
			return -1;
		break;
	case NO_HOLD_MASTER:
		if (sht20_readData_noHlodMaster(pSHT->hardware.pI2C,(uint8_t) SHT20_CMD_RH_MSURMT, &RxBuffer)!=0)
			return -2;
		break;

	default:
		return -3;
	}

	//check CRC
	CRC8=RxBuffer[SHT20_LENGTH_OF_DATA-1];
	if (SHT20_checkCRC(RxBuffer,(SHT20_LENGTH_OF_DATA-1))!=0)
		return -4;

	*pRecvHmty=SHT20_conversionHumidity(&RxBuffer);

	#ifdef PRINT_DEBUG
		printf_hex("humidity data is ", RxBuffer, SHT20_LENGTH_OF_DATA);
		printf("humidity is %d\r\n",*pRecvHmty);
	#endif
	return 0;

}



int8_t  SHT20_conversionTemperature(uint8_t *data){
	/*
	 * 	The temperature T is calculated by inserting temperature
		signal output S T into the following formula (result in °C), no
		matter which resolution is chosen:
		T=-46.85+(175.72*(St/2^16))
		2^16=65536
	 */
	uint16_t St;
	St=(data[0]<<8)|data[1];

	return (-46.85)+(175.72*((float)St/65536));
}


uint8_t SHT20_conversionHumidity(uint8_t *data){
	/*
	 * 	With the relative humidity signal output S RH the relative
		humidity RH is obtained by the following formula (result in
		%RH), no matter which resolution is chosen:
		RH=-6+125(Srh/2^16)
	 */
	uint16_t Srh;
	Srh=(data[0]<<8)|data[1];

	return (-6)+(125*((float)Srh/65536));
}
int SHT20_checkCRC(uint8_t *data,uint8_t len){
	//check CRC
	return 0;
	uint8_t CRC8=data[len-1];
	if (SHT20_SHT20_calculateCRC(data,(len-1))!=CRC8)
		return -1;

	return 0;
}


uint8_t SHT20_calculateCRC(uint8_t *data,uint8_t len){
	//
	uint8_t CRC8=0;
	return CRC8;
}
int sht20_setBitUserRegister(SHT20_HandelTypedef *pSHT,uint8_t pTransData){

	uint8_t recvReg;

	if (sht20_readUserRegister(pSHT->hardware.pI2C, &recvReg)!=0)
		return-1;

	recvReg|=pTransData;

	if (sht20_writeUserRegister(pSHT->hardware.pI2C, recvReg)!=0)
		return -2;

	return 0;
}

int sht20_clearBitUserRegister(SHT20_HandelTypedef *pSHT,uint8_t pTransData){

	uint8_t recvReg;

	if (sht20_readUserRegister(pSHT->hardware.pI2C, &recvReg)!=0)
		return-1;

	recvReg&=~pTransData;
	if (sht20_writeUserRegister(pSHT->hardware.pI2C, &recvReg)!=0)
		return -2;

	return 0;
}

int sht20_writeUserRegister(SHT20_HandelTypedef *pSHT,uint8_t *pTransData){

	return sht20_writeRegister(pSHT->hardware.pI2C,(uint8_t) SHT20_CMD_WRITE_REGISTER, pTransData, 1);
}

int sht20_readUserRegister(SHT20_HandelTypedef *pSHT,uint8_t *pRecvData){

	return sht20_readRegister(pSHT->hardware.pI2C,(uint8_t)SHT20_CMD_READ_REGISTER,pRecvData,1);
}



int sht20_writeRegister(I2C_HandleTypeDef *pI2C,uint8_t CMD,uint8_t *pTrnsData,uint8_t size){

	HAL_StatusTypeDef ret=HAL_ERROR;
	uint8_t *TXBuffer=(uint8_t *)malloc((size+1)* sizeof(uint8_t));

	TXBuffer[0]=(uint8_t)CMD;
	memcpy(&TXBuffer[1],pTrnsData,size);

	ret=HAL_I2C_Master_Transmit(pI2C, SHT20_I2C_ADDRESS, TXBuffer, size+1, 20);

	free(TXBuffer);
	if (ret!=HAL_OK)
		return -1;



	return 0;

}

int sht20_readRegister(I2C_HandleTypeDef *pI2C,uint8_t CMD,uint8_t *pRecvData,uint8_t size){


	if (HAL_I2C_Master_Transmit(pI2C, SHT20_I2C_ADDRESS,&CMD, 1, 20)!=HAL_OK)
		return -1;
	HAL_Delay(1);
	if (HAL_I2C_Master_Receive(pI2C, SHT20_I2C_ADDRESS, pRecvData, size, 20)!=HAL_OK)
		return -2;
	return 0;
}

int sht20_readData_hlodMaster(I2C_HandleTypeDef *pI2C,uint8_t CMD,uint8_t *pRecvData){

	CMD|=SHT20_HOLD_MASTER;

	if (HAL_I2C_Master_Transmit(pI2C, SHT20_I2C_ADDRESS, &CMD, 1, 100)!=HAL_OK)
		return -1;
	if (HAL_I2C_Master_Receive(pI2C, SHT20_I2C_ADDRESS, pRecvData, SHT20_LENGTH_OF_DATA, 200)!=HAL_OK)
		return -2;

	return 0;
}


int sht20_readData_noHlodMaster(I2C_HandleTypeDef *pI2C,uint8_t CMD,uint8_t *pRecvData){

	CMD|=SHT20_NO_HOLD_MASTER;

	if (HAL_I2C_Master_Transmit(pI2C, SHT20_I2C_ADDRESS, &CMD, 1, 50)!=HAL_OK)
		return -1;
	HAL_Delay(200);
	if (HAL_I2C_Master_Receive(pI2C, SHT20_I2C_ADDRESS, pRecvData, SHT20_LENGTH_OF_DATA, 50)!=HAL_OK)
		return -2;

	return 0;
}
