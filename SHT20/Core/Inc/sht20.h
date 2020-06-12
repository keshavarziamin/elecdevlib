/*
 * sht20.h
 *
 * Datasheet: https://www.sensirion.com/fileadmin/user_upload/customers/sensirion/Dokumente/2_Humidity_Sensors/Datasheets/Sensirion_Humidity_Sensors_SHT20_Datasheet.pdf
 *  Created on: Jun 12, 2020
 *      Author: amin
 */

#ifndef SHT20_H_
#define SHT20_H_

#include "gpio.h"
#include "i2c.h"




#define SHT20_I2C_ADDRESS		0x80
#define SHT20_I2C_WRITE_MODE(X)	((X)&(~0xFE))
#define SHT20_I2C_READ_MODE(X)	((X)|(~0x01))

#define SHT20_CMD_T_MSURMT_HM 		0xE3	//Trigger T  measurement (hold master) 		'11100011'
#define SHT20_CMD_RH_MSURMT_HM 		0xE5 	//Trigger RH measurement (hold master) 		'11100101'
#define SHT20_CMD_T_MSURMT_NHM 		0xF3	//Trigger T  measurement (no hold master) 	'11110011'
#define SHT20_CMD_RH_MSURMT_NHM 	0xF5 	//Trigger RH measurement (no hold master)	'11110101'
#define SHT20_CMD_WRITE_REGISTER	0xE6	//Write user register 	 '11100110'
#define SHT20_CMD_READ_REGISTER		0xE7	//Read user register     '11100111'
#define SHT20_CMD_RESET				0xFE	//soft reset'11111110'












//////////////////////////////////////////////////////functions////////////////////////////////////

int sht20_writeUserRegister(I2C_HandleTypeDef *pI2C,uint8_t *pRecvData);
int sht20_readUserRegister(I2C_HandleTypeDef *pI2C,uint8_t *pRecvData);



int sht20_readRegister(I2C_HandleTypeDef *pI2C,uint8_t CMD,uint8_t *pRecvData,uint8_t size);
int sht20_writeRegister(I2C_HandleTypeDef *pI2C,uint8_t CMD,uint8_t *pTrnsData,uint8_t size);


#endif /* SHT20_H_ */
