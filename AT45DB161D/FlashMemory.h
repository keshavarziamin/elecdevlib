#ifndef FlashMemory_H
#define FlashMemory_H

#include "stm32f4xx_hal.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "spi.h"
#include "glob_func.h"



//define

#define SIZE_SECTOR			16
#define SIZE_BLOCK			512
#define SIZE_PAGE				4096

#define MAX_WRITE_DATA  1000

//opcode
//READ COMMAND
#define OPCODE_READ_MAIN_MEMORY_PAGE  										0XD2
#define OPCODE_READ_CONTINUOUS_AREA_LEGACY_COMMAND				0XE8
#define OPCODE_READ_CONTINUOUS_AREA_LOW_FERQUENCY					0X03
#define OPCODE_READ_CONTINUOUS_AREA_HIGH_FERQUENCY				0X0B
#define OPCODE_READ_BUFFER_1_LOW_FERQENCY									0XD1
#define OPCODE_READ_BUFFER_2_LOW_FERQENCY									0XD3
#define OPCODE_READ_BUFFER_1															0XD4			
#define OPCODE_READ_BUFFER_2															0XD6

//Program and Erase Commands
#define OPCODE_WRITE_BUFFER_1															0X84	
#define OPCODE_WRITE_BUFFER_2															0X87					
#define OPCODE_PROGRAM_BUFFER1_TO_MMP_WITH_BUILT_IN				0X83					
#define OPCODE_PROGRAM_BUFFER2_TO_MMP_WITH_BUILT_IN				0X86						
#define OPCODE_PROGRAM_BUFFER1_TO_MMP_WITHOUT_BUILT_IN		0X88									
#define OPCODE_PROGRAM_BUFFER2_TO_MMP_WITHOUT_BUILT_IN		0X89									
#define OPCODE_ERASE_PAGE																	0X81										
#define OPCODE_ERASE_BLOCK																0X50								
#define OPCODE_ERASE_SECTOR																0X7C											
#define OPCODE_ERASE_CHIP																	0XC7//0X94,0X80,0X9A												
#define OPCODE_PROGRAM_MMP_THROUGH_BUFFER1								0X82							
#define OPCODE_PROGRAM_MMP_THROUGH_BUFFER2								0X85

//Protection and Security Commands
#define OPCODE_ENABLE_SECTOR_PROTECTION										0X3D2A7FA9
#define OPCODE_DISABLE_SECTOR_PROTECTION									0x9A7F2A3D		//0x9A7F2A3D
#define OPCODE_ERASE_SECTOR_PROTECTION_REGISTER						0X3D2A7FCF				
#define OPCODE_PROGRAM_SECTOR_PROTECTION_REGISTER					0X3D2A7FFC				
#define OPCODE_READ_SECTOR_PROTECTION_REGISTER						0X32										
#define OPCODE_SECTOR_LOCKDOWN														0X3D2A7430						
#define OPCODE_READ_SECTOR_LOCKDOWN_REGISTER							0X35						
#define OPCODE_PROGRAM_SECURITY_REGISTER									0X9B000000						
#define OPCODE_READ_SECURITY_REGISTER											0X77					

//Additional Commands

#define OPCODE_TRANSFER_MMP_TO_BUFFER1										0X53
#define OPCODE_TRANSFER_MMP_TO_BUFFER2										0X55
#define OPCODE_COMPARE_MMP_TO_BUFFER1											0X60		
#define OPCODE_COMPARE_MMP_TO_BUFFER2											0X61				
#define OPCODE_REWRITE_PAGE_THROUGH_BUFFER1_AUTO					0X58				
#define OPCODE_REWRITE_PAGE_THROUGH_BUFFER2_AUTO					0X59		
#define OPCODE_DEEP_POWER_DOWN														0XB9							
#define OPCODE_RESUME_FROM_DEEP_POWER_DOWN								0XAB		
#define OPCODE_READ_STATUS_REGISTER												0XD7				
#define OPCODE_READ_DEVICE_ID															0X9F

//status register
/*
If bit 7 is a 1, then the device is
not busy and is ready to accept the next command. If bit 7 is a 0, then the device is in a busy
state.
*/
#define STATUS_REGISTER_READY					0x80 
/*
If bit 6 is a 0, then the data in the main memory page matches the
data in the buffer. If bit 6 is a 1, 
then at least one bit of the data in the main memory page does
not match the data in the buffer.
*/
#define STATUS_REGISTER_COMP					0x40
/*
A logic 1 indicates that sector protection has been enabled and 
logic 0 indicates that sector protection has been disabled.
*/
#define STATUS_REGISTER_PROCTECT			0x20
/*
If bit 0 is a 1, then the page size is set to 512 bytes. If bit 0 is a 0,
then the page size is set to 528 bytes.
*/
#define STATUS_REGISTER_PAGE_SIZE			0x01 




union TX_ADDRESS
{
	uint8_t byte[4];
	uint32_t word;
};	

typedef struct {
	int32_t write;
	int32_t read;
	int16_t count;
}__attribute__((packed)) FLASH_IndextTypedef;
typedef struct 
{
	uint8_t statusRegister;
	uint16_t information;
	FLASH_IndextTypedef Index;
}FLASH_MEM_RegidterTypedef;
typedef struct
{
	GPIO_TypeDef* GPIOx;
	uint16_t Pin;
}FLASH_MEM_GPIOTypedef;

typedef struct 
{
	SPI_HandleTypeDef spi;
	FLASH_MEM_GPIOTypedef WP;
	FLASH_MEM_GPIOTypedef Reset;
	FLASH_MEM_GPIOTypedef CS;
	
}FALSH_MEM_ConfigTypedef;




typedef struct 
{
	FALSH_MEM_ConfigTypedef Config;
	FLASH_MEM_RegidterTypedef Register;
}FLASH_MEM_HandleTypedef;



/**
*@breif:
*@param:
*@param:
*@retval
*/

HAL_StatusTypeDef FLASH_init(FLASH_MEM_HandleTypedef *FM);


/**
*@breif:
*@param:
*@param:
*@retval
*/
HAL_StatusTypeDef FLASH_writeFIFO(FLASH_MEM_HandleTypedef *FM,void *Data,uint8_t sizeOfData);
/**
*@breif:
*@param:
*@param:
*@retval
*/
HAL_StatusTypeDef FLASH_readFIFO(FLASH_MEM_HandleTypedef *FM,void *Data,uint8_t sizeOfData);



/**
*@breif:
*@param:
*@param:
*@retval
*/
HAL_StatusTypeDef FLASH_readData(FLASH_MEM_HandleTypedef *FM,uint16_t pageAddr,uint16_t byteAddr,uint8_t *RxData,uint8_t lengthOfRx);
/**
*@breif:
*@param:
*@param:
*@retval
*/
HAL_StatusTypeDef FLASH_writeData(FLASH_MEM_HandleTypedef *FM,uint16_t pageAddr,uint16_t byteAddr,uint8_t *TxData,uint8_t lengthOfTx);


/**
*@breif:
*@param:
*@param:
*@retval
*/
HAL_StatusTypeDef FLASH_readStatusRegister(FLASH_MEM_HandleTypedef *FM,uint8_t *SR);
/**
*@breif:
*@param:
*@param:
*@retval
*/
HAL_StatusTypeDef FLASH_enableSectorProtection(FLASH_MEM_HandleTypedef *FM);
/**
*@breif:
*@param:
*@param:
*@retval
*/
HAL_StatusTypeDef FLASH_disableSectorProtection(FLASH_MEM_HandleTypedef *FM);
/**
*@breif:
*@param:
*@param:
*@retval
*/

HAL_StatusTypeDef FLASH_getInformation(FLASH_MEM_HandleTypedef *FM,uint8_t *info);
/**
*@breif:
*@param:
*@param:
*@retval
*/
HAL_StatusTypeDef FLASH_getData(FLASH_MEM_HandleTypedef *FM,uint8_t *transmitData,uint16_t lengthOfTX,uint8_t *receivedData,uint16_t lengthOfDataa);
/**
*@breif:
*@param:
*@param:
*@retval
*/

HAL_StatusTypeDef FLASH_sendData(FLASH_MEM_HandleTypedef *FM,uint8_t *transmitData,uint16_t lengthOfTX);

#endif
