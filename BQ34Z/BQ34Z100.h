#ifndef BQ34Z100_H
#define BQ34Z100_H

#include "stm32f4xx_hal.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "i2c.h"
#include "glob_func.h"
#include "math.h"

#define BQ_I2C_ADDRESS 0xAA

//commands
#define 	BQ_CMD_CONTROL 									0x0001
#define 	BQ_CMD_STATEOFCHARGE 						0x02
#define 	BQ_CMD_MAXERROR 								0x03
#define 	BQ_CMD_REMAININGCAPACITY 				0x0405
#define 	BQ_CMD_FULLCHARGECAPACITY 			0x0607
#define 	BQ_CMD_VOLTAGE 									0x0809
#define 	BQ_CMD_AVERAGECURRENT 					0x0A0B
#define 	BQ_CMD_TEMPERATURE 							0x0C0D
#define 	BQ_CMD_FLAGS 										0x0E0F
#define 	BQ_CMD_CURRENT 									0x1011
#define 	BQ_CMD_FLAGSB 									0x1213

//Control() Subcommands
#define 	BQ_CTRL_SUB_CONTROL_STATUS 			0x0000
#define 	BQ_CTRL_SUB_DEVICE_TYPE 				0x0001
#define 	BQ_CTRL_SUB_FW_VERSION 					0x0002
#define 	BQ_CTRL_SUB_HW_VERSION					0x0003
#define 	BQ_CTRL_SUB_RESET_DATA 					0x0005
#define 	BQ_CTRL_SUB_PREV_MACWRITE 			0x0007
#define 	BQ_CTRL_SUB_CHEM_ID 						0x0008
#define 	BQ_CTRL_SUB_BOARD_OFFSET				0x0009
#define 	BQ_CTRL_SUB_CC_OFFSET 					0x000A
#define 	BQ_CTRL_SUB_CC_OFFSET_SAVE 			0x000B
#define 	BQ_CTRL_SUB_DF_VERSION 					0x000C
#define 	BQ_CTRL_SUB_SET_FULLSLEEP 			0x0010
#define 	BQ_CTRL_SUB_STATIC_CHEM_CHKSUM 	0x0017
#define 	BQ_CTRL_SUB_SEALED 							0x0020
#define 	BQ_CTRL_SUB_IT_ENABLE 					0x0021
#define 	BQ_CTRL_SUB_CAL_ENABLE 					0x002D
#define 	BQ_CTRL_SUB_RESET 							0x0041
#define 	BQ_CTRL_SUB_EXIT_CAL 						0x0080
#define 	BQ_CTRL_SUB_ENTER_CAL 					0x0081
#define 	BQ_CTRL_SUB_OFFSET_CAL 					0x0082

#define 	BQ_CTRL_SUB_UNSEALED						0x36720414




//Extended Data Commands

#define BQ_CMD_EXT_AVERAGETIMETOEMPTY					0x1819
#define BQ_CMD_EXT_AVERAGETIMETOFULL					0x1A1B
#define BQ_CMD_EXT_PASSEDCHARGE								0x1C1D
#define BQ_CMD_EXT_DOD0TIME										0x1E1F
#define BQ_CMD_EXT_AVAILABLEENERGY						0x2425
#define BQ_CMD_EXT_AVERAGEPOWER								0x2627
#define BQ_CMD_EXT_SERIAL NUMBER							0x2829
#define BQ_CMD_EXT_INTERNAL_TEMPERATURE				0x2A2B
#define BQ_CMD_EXT_CYCLECOUNT									0x2C2D
#define BQ_CMD_EXT_STATEOFHEALTH							0x2E2F
#define BQ_CMD_EXT_CHARGEVOLTAGE							0x3031
#define BQ_CMD_EXT_CHARGECURRENT							0x3233
#define BQ_CMD_EXT_PACKCONFIGURATION					0x3A3B
#define BQ_CMD_EXT_DESIGNCAPACITY							0x3C3D
#define BQ_CMD_EXT_DATAFLASHCLASS							0x3E
#define BQ_CMD_EXT_DATAFLASHBLOCK							0x3F
#define BQ_CMD_EXT_AUTHENTICATE_BLOCKDATA			0x40
#define BQ_CMD_EXT_AUTHENTICATECHECKSUM				0x54
#define BQ_CMD_EXT_BLOCKDATA									0x54
#define BQ_CMD_EXT_BLOCKDATACHECKSUM					0x60
#define BQ_CMD_EXT_BLOCKDATACONTROL						0x61
#define BQ_CMD_EXT_GRIDNUMBER 								0x62
#define BQ_CMD_EXT_LEARNEDSTATUS 							0x63
#define BQ_CMD_EXT_BLOCKDATACONTROL						0x61
#define BQ_CMD_EXT_GRIDNUMBER 								0x62
#define BQ_CMD_EXT_LEARNEDSTATUS 							0x63
#define BQ_CMD_EXT_DOD_EOC 										0x6465
#define BQ_CMD_EXT_QSTART 										0x6667
#define BQ_CMD_EXT_TRUERC 										0x6869
#define BQ_CMD_EXT_TRUEFCC 										0x6A6B
#define BQ_CMD_EXT_STATETIME 									0x6C6D
#define BQ_CMD_EXT_QMAXPASSED 								0x6E6F


/*Data Flash Summary*/

#define BQ_SUBCLASS_ID_CONFIGURATION_DATA			48
//subclasss
#define BQ_OFFSET_DESIGNCAPACITY						11		
#define BQ_OFFSET_DESIGNENERGY							13		
#define BQ_OFFSET_CELLCHARGEVOLTAGE_T1T2		17		
#define BQ_OFFSET_CELLCHARGEVOLTAGE_T2T3		19		
#define BQ_OFFSET_CELLCHARGEVOLTAGE_T3T4		21		
#define BQ_OFFSET_CELLCHARGECURRENT_T1T2		23		
#define BQ_OFFSET_CELLCHARGECURRENT_T2T3		24		
#define BQ_OFFSET_CELLCHARGECURRENT_T3T4		25		                                               
#define BQ_OFFSET_DESIGNENERGYSCALE					30

#define BQ_SUBCLASS_ID_CONFIGURATION_REGISTER 64
//
#define BQ_OFFSET_PACKCONFIGURATION					0
#define BQ_OFFSET_PACKCONFIGURATION_B				2
#define BQ_OFFSET_PACKCONFIGURATION_C				3
#define BQ_OFFSET_NUMBEROFCELL							7

#define BQ_SUBCLASS_ID_CALIBRARTION_DATA 		104
//
#define BQ_OFFSET_VOLTAGEDIVIDER					14




//pack configuration
#define BQ_BITS_VOLTSEL			0x08
#define BQ_BITS_SCALE				0x20
#define BQ_BITS_CAL_EN			0x40


typedef struct BQ_BatteryTypedef
{
	uint8_t stateOfCharge;
	uint16_t fullChargeCapacity;
	uint8_t numberOfCell;
	uint8_t scale;

	uint16_t voltage;
	uint16_t voltageDevider;
	uint16_t designCapacity;
	uint16_t designEnergy;
	uint16_t cellChargeVoltage;
	uint16_t averageCurrent;
	uint16_t temperature;
	uint16_t current;
}BQ_BatteryTypedef;

typedef struct
{
	uint8_t blockData[32];
	uint16_t flags;
	uint16_t flagsB;
	uint16_t controlStatus;
	uint16_t PackConfiguration;
}BQ_RegisterTypedef;
typedef struct 
{
	I2C_HandleTypeDef* i2c;
	
}	BQ_ConfigTypedef;



typedef struct 
{
	BQ_ConfigTypedef Config;
	BQ_BatteryTypedef Battery;
	BQ_RegisterTypedef reg;
}	BQ_HandleTypedef;

int BQ_setScale(BQ_HandleTypedef *pBQ);
int BQ_setNumberOfCell(BQ_HandleTypedef *pBQ,uint8_t number);
int BQ_setVoltageDivider(BQ_HandleTypedef *pBQ);
int BQ_updateDesignCapacity(BQ_HandleTypedef *pBQ);
int BQ_updateDesignEnergy(BQ_HandleTypedef *pBQ);
int BQ_updateCellChargeVolatge(BQ_HandleTypedef *pBQ);
int BQ_calibrateVoltageDivider(BQ_HandleTypedef *pBQ,uint16_t appliedVoltage);

int BQ_clearBitFlashBlock(BQ_HandleTypedef *pBQ,uint8_t subClass,uint8_t offset,uint8_t Data);
int BQ_setBitFlashBlock(BQ_HandleTypedef *pBQ,uint8_t subClass,uint8_t offset,uint8_t Data);
int BQ_writeFlashBlock(BQ_HandleTypedef *pBQ,uint8_t subClass,uint8_t offset,uint8_t *pData,uint8_t lengthOfData);
int BQ_readFlashBlock(BQ_HandleTypedef *pBQ,uint8_t subClass,uint8_t offset,uint8_t *pData,uint8_t lengthOfData);

uint8_t BQ_calculateChecksum(BQ_HandleTypedef *pBQ);
int BQ_writeCheckSum(BQ_HandleTypedef *pBQ,uint8_t checkSum);
int BQ_readCheckSum(BQ_HandleTypedef *pBQ,uint8_t *checkSum);
int BQ_selectOffset(BQ_HandleTypedef *pBQ,uint8_t offset);
int BQ_accessSubClass(BQ_HandleTypedef *pBQ,uint8_t subClass);
int BQ_setUNSEAL(BQ_HandleTypedef *pBQ);
int BQ_setSEAL(BQ_HandleTypedef *pBQ);
int BQ_reset(BQ_HandleTypedef *pBQ);

int BQ_writeCTRL(BQ_HandleTypedef *pBQ,uint16_t subCMD);
int BQ_readCTRL(BQ_HandleTypedef *pBQ,uint16_t subCMD,uint16_t *RxData);
int BQ_init(BQ_HandleTypedef *pBQ);
int BQ_writeRegister(BQ_HandleTypedef *pBQ,uint8_t *CMD,uint8_t lengthOfCMD,uint8_t *TxData,uint8_t lengthTxData);
int BQ_readRegister(BQ_HandleTypedef *pBQ,uint8_t *CMD,uint8_t lenghtOfCMD,uint8_t *RxData,uint8_t lengthRxData);
int BQ_getVlotage(BQ_HandleTypedef *pBQ);
int BQ_getDesignCapacity(BQ_HandleTypedef *pBQ,uint16_t *designCapacity);
int BQ_getStateOfCharge(BQ_HandleTypedef *pBQ);
#endif