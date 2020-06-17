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

	#ifdef PRINT_DEBUG==1
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

	#ifdef PRINT_DEBUG==1
		printf("the value of userReg is %x\r\n",userReg);
	#endif

	pSHT->reg.userRegister=recvuserReg;
	return 0;
}




int sht20_getTemperature(SHT20_HandelTypedef *pSHT,int8_t *pRecvTemp,HoldMasterMode mode ){

	uint8_t RxBuffer[SHT20_LENGTH_OF_DATA];

	switch (mode){
	case NO_HOLD_MASTER:
		if ()
	}




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

	#ifdef PRINT_DEBUG==1
		printf_hex("send data to sht20  => ", TXBuffer, size+1);
	#endif


	ret=HAL_I2C_Master_Transmit(pI2C, SHT20_I2C_ADDRESS, TXBuffer, size+1, 20);

	#ifdef PRINT_DEBUG==1
		printf_hex("sending return is %d \r\n",ret);
	#endif

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

int sht20_readData_hlodMaster(I2C_HandleTypeDef *pI2C,uint8_t CMD,int8_t *pRecvData){

	if (HAL_I2C_Master_Transmit(pI2C, SHT20_I2C_ADDRESS, &CMD, 1, 100)!=0)
		return -1;
	if (HAL_I2C_Master_Receive(pI2C, SHT20_I2C_ADDRESS, pRecvData, SHT20_LENGTH_OF_DATA, 200)!=0)
		return -2;
	return 0;
}
