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

#define PRINT_DEBUG 1


//////////////////////////////////////define//////////////////////////////////////////////////////////





#define SHT20_I2C_ADDRESS		0x80
#define SHT20_I2C_WRITE_MODE(X)	((X)&(~0xFE))
#define SHT20_I2C_READ_MODE(X)	((X)|(~0x01))

#define SHT20_LENGTH_OF_DATA	3

#define SHT20_CMD_T_MSURMT_HM 		0xE3	//Trigger T  measurement (hold master) 		'11100011'
#define SHT20_CMD_RH_MSURMT_HM 		0xE5 	//Trigger RH measurement (hold master) 		'11100101'
#define SHT20_CMD_T_MSURMT_NHM 		0xF3	//Trigger T  measurement (no hold master) 	'11110011'
#define SHT20_CMD_RH_MSURMT_NHM 	0xF5 	//Trigger RH measurement (no hold master)	'11110101'
#define SHT20_CMD_WRITE_REGISTER	0xE6	//Write user register 	 '11100110'
#define SHT20_CMD_READ_REGISTER		0xE7	//Read user register     '11100111'
#define SHT20_CMD_RESET				0xFE	//soft reset'11111110'

#define SHT20_MEATUREMENT_RESULATION_12_14		0x00
#define SHT20_MEATUREMENT_RESULATION_08_12		0x01
#define SHT20_MEATUREMENT_RESULATION_10_13		0x80
#define SHT20_MEATUREMENT_RESULATION_11_11		0x81
#define SHT20_OTP_RELOAD						0x02
#define SHT20_ON_CHIP_HEATER					0x04
////////////////////////////////////////////struct////////////////////////////////////////////////////
typedef struct SHT20_HardwareTypedef{
	I2C_HandleTypeDef *pI2C;
}SHT20_HardwareTypedef;

typedef struct SHT20_ConfigTypedef{
	FunctionalState onChipHeater;
	FunctionalState OTPReload;
	uint8_t meaturementResulation;
}SHT20_ConfigTypedef;

typedef struct SHT20_RegisterTypedef{
	uint8_t userRegister;
	int8_t temperature;
	uint8_t humidity;
}SHT20_RegisterTypedef;


typedef struct SHT20_HandelTypedef{
	SHT20_HardwareTypedef 	hardware;
	SHT20_ConfigTypedef 	configuration;
	SHT20_RegisterTypedef	reg;
}SHT20_HandelTypedef;


typedef enum{
	NO_HOLD_MASTER=0U,
	HOLD_MASTER=!NO_HOLD_MASTER
}HoldMasterMode;




//////////////////////////////////////////////////////functions////////////////////////////////////

int sht20_init(SHT20_HandelTypedef *pSHT);

int sht20_setBitUserRegister(SHT20_HandelTypedef *pSHT,uint8_t pTransData);
int sht20_clearBitUserRegister(SHT20_HandelTypedef *pSHT,uint8_t pTransData);

int sht20_writeUserRegister(SHT20_HandelTypedef *pSHT,uint8_t *pTransData);
int sht20_readUserRegister(SHT20_HandelTypedef 	*pSHT,uint8_t *pRecvData);





int sht20_readRegister(I2C_HandleTypeDef *pI2C,uint8_t CMD,uint8_t *pRecvData,uint8_t size);
int sht20_writeRegister(I2C_HandleTypeDef *pI2C,uint8_t CMD,uint8_t *pTrnsData,uint8_t size);

int sht20_readData_hlodMaster(I2C_HandleTypeDef *pI2C,uint8_t CMD,int8_t *pRecvData);

#endif /* SHT20_H_ */
