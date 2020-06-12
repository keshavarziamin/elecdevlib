/*
 * sht20.c
 *
 *  Created on: Jun 12, 2020
 *      Author: amin
 */


#include "sht20.h"





int sht20_writeUserRegister(I2C_HandleTypeDef *pI2C,uint8_t *pTransData){
	return sht20_writeRegister(pI2C, SHT20_CMD_WRITE_REGISTER, pTransData, 1);
}

int sht20_readUserRegister(I2C_HandleTypeDef *pI2C,uint8_t *pRecvData){

	return sht20_readRegister(pI2C,(uint8_t)SHT20_CMD_READ_REGISTER,pRecvData,1);
}



int sht20_writeRegister(I2C_HandleTypeDef *pI2C,uint8_t CMD,uint8_t *pTrnsData,uint8_t size){

	HAL_StatusTypeDef ret=HAL_ERROR;
	uint8_t *TXBuffer=(uint8_t *)malloc((size+1)* sizeof(uint8_t));

	TXBuffer[0]=(uint8_t)CMD;
	memcpy(&TXBuffer[1],pTrnsData,size);

	ret=HAL_I2C_Master_Transmit(pI2C, SHT20_I2C_ADDRESS, TXBuffer, size, 20);

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
