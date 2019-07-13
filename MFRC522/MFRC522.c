#include "MFRC522.h"

MFRC522_HandleTypeDef MF_typedef;


//private functions
static HAL_StatusTypeDef MFRC522_readData(uint8_t Addr,uint8_t *Data,uint8_t Size);
static HAL_StatusTypeDef MFRC522_writeData(uint8_t Addr,uint8_t *Data,uint8_t Size);
void MFRC522_init(MFRC522_HandleTypeDef MF_config)
{
	/*
	@braif: initializing the MFRC522
					1.set hardware port to connect to device
					2.set default parameter value 
	*/
	MF_typedef.SPI=MF_config.SPI;
	MF_typedef.CS_port=MF_config.CS_port;
	MF_typedef.CS_Pin=MF_config.CS_Pin;
}

void MFRC522_reset(void)
{
	/*
	@braif: reset the all registers of MFRC522
	*/
	
}

void MFRC522_setDefault(void)
{
	
}

static HAL_StatusTypeDef MFRC522_readData(uint8_t Addr,uint8_t *Data,uint8_t Size)
{
	/*
	@brief:
		*Reading data using SPI requires the byte order shown in Table to be used. It is possible
		*to read out up to n-data bytes.
		*The first byte sent defines both the mode and the address.
		*NOTE:The MSB must be sent first.
	table:
		|Line >> 	Byte0 	|		Byte1 |	Byte2 		|	To |	Byten 	|	Byten+1
	--------------------------------------------------------------------
		|MOSI >> address0	| address1| address2 	|	... |	addressn 	|		00
	--------------------------------------------------------------------
		|MISO >> 	X[1] 		|	data0 	|	data1 		|	... |	datan-1 	|	data n
	
		X = Do not care.
	
	*/
	HAL_StatusTypeDef SPI_status;
	uint8_t Addr_Data[20];
	
	/*
	The address byte must meet the following format.
	The MSB of the first byte defines the mode used. To read data from the MFRC522 the
	MSB is set to logic 1. To write data to the MFRC522 the MSB must be set to logic 0. Bits 6
	to 1 define the address and the LSB is set to logic 0.
	*/
	Addr_Data[0]=((Addr<<1)&0x7E)|0x80;
	
	
	//array of address
	for (int m=0;m<Size;m++)
		Addr_Data[m+1]=Addr_Data[m]+1;
	Addr_Data[Size]=0x00;
	
	//Transmit & Receive redata
	MFRC522_ChipSelect_ON();
	SPI_status=HAL_SPI_TransmitReceive(&MF_typedef.SPI,Addr_Data,Data,Size+1,100);
	MFRC522_ChipSelect_OFF();
	
	if (SPI_status!=HAL_OK)
		return HAL_ERROR;
	return HAL_OK;
	
}

static HAL_StatusTypeDef MFRC522_writeData(uint8_t Addr,uint8_t *Data,uint8_t Size)
{
	/*
	@brief:
		*To write data to the MFRC522 using SPI requires the byte order shown in Table. It is
		*possible to write up to n data bytes by only sending one address byte.
		*The first send byte defines both the mode and the address byte.
		*NOTE:The MSB must be sent first.
		table:
			|Line >> 	Byte0 	|		Byte1 |	Byte2 		|	To |	Byten 		|	Byten+1
		--------------------------------------------------------------------
			|MOSI >> address0	| 	data0	| data1 		|	... |	data n-1 	|	 data n
		--------------------------------------------------------------------
			|MISO >> 	X[1] 		|		X[1] 	|		X[1] 		|	... |		X[1] 		|	X[1]
	
			X = Do not care.
	*/
	HAL_StatusTypeDef SPI_status;
	uint8_t write_Data[20];
	/*
	The address byte must meet the following format.
	The MSB of the first byte defines the mode used. To read data from the MFRC522 the
	MSB is set to logic 1. To write data to the MFRC522 the MSB must be set to logic 0. Bits 6
	to 1 define the address and the LSB is set to logic 0.
	*/
	write_Data[0]=((Addr<<1)&0x7E)|0x80;
	
	for (int m=0;m<Size;m++)
		write_Data[m+1]=Data[m];
	
	MFRC522_ChipSelect_ON();
	HAL_SPI_Transmit(&MF_typedef.SPI,write_Data,Size+1,100);
	MFRC522_ChipSelect_OFF();
	
	if (SPI_status!=HAL_OK)
		return HAL_ERROR;
	return HAL_OK;
	
}