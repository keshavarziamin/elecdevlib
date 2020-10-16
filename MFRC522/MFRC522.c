#include "MFRC522.h"

MFRC522_HandleTypeDef MF_typedef;


//private functions

void MFRC522_init(MFRC522_HandleTypeDef MF_config)
{
	/*
	@braif: initializing the MFRC522
					1.set hardware port to connect to device
					2.reset device
					3.set default parameter value 
					4.turn on antenna
	*/
	//config SPI port
	MF_typedef.SPI=MF_config.SPI;
	MF_typedef.CS_port=MF_config.CS_port;
	MF_typedef.CS_Pin=MF_config.CS_Pin;
	
	MFRC522_reset();
	//set default parameter
	MFRC522_setDefault();
	MFRC522_antennaOn();

}

HAL_StatusTypeDef MFRC522_reset(void)
{
	/*
	@braif: reset the all registers of MFRC522
	@return: HAL_OK,HAL_ERROR
	*/
	if(MFRC522_writeReg((uint8_t *)MFRC522_REG_COMMAND,(uint8_t *)COMMAND_RESET,1)!=HAL_OK)
		return HAL_ERROR;
	
	return HAL_OK;
}

void MFRC522_setDefault(void)
{
	
	//Timer: TPrescaler*TreloadVal/6.78MHz = 24ms
	MFRC522_writeReg((uint8_t *)MFRC522_REG_T_MODE,(uint8_t *)0x8D,1); //Tauto=1; f(Timer) = 6.78MHz/TPreScaler
	MFRC522_writeReg((uint8_t *)MFRC522_REG_T_PRESCALER,(uint8_t *)0x3E,1);
	
	MFRC522_writeReg((uint8_t *)MFRC522_REG_T_RELOAD_H,(uint8_t*)0x00,1);
	MFRC522_writeReg((uint8_t *)MFRC522_REG_T_RELOAD_L,(uint8_t*)30,1);
	
	MFRC522_writeReg((uint8_t *)MFRC522_REG_TX_AUTO,(uint8_t *)0x40,1);
	MFRC522_writeReg((uint8_t *)MFRC522_REG_MODE,(uint8_t *)0x3D,1);
	
}
HAL_StatusTypeDef MFRC522_antennaOn(void)
{
	/*
	@brief: turn on antenna 
	
	@return: HAL_OK,HAL_ERROR
	*/
	uint8_t wr_Data=0;
	wr_Data|=(TX_CONTROL_Tx2RFEn |TX_CONTROL_Tx2RFEn); //(re_Data|0x03)
	if (MFRC522_setBitReg((uint8_t *)MFRC522_REG_TX_CONTROL,wr_Data)!=HAL_OK)
		return HAL_ERROR;
	
	return HAL_OK;
}
HAL_StatusTypeDef MFRC522_antennaOFF(void)
{
	/*
	@brief: turn off antenna 	
	@return: HAL_OK,HAL_ERROR
	*/
	uint8_t wr_Data=0;
	wr_Data|=(TX_CONTROL_Tx2RFEn |TX_CONTROL_Tx2RFEn); //(re_Data|0x03)
	if (MFRC522_clearBitReg((uint8_t *)MFRC522_REG_TX_CONTROL,wr_Data)!=HAL_OK)
		return HAL_ERROR;
	
	return HAL_OK;
}
HAL_StatusTypeDef MFRC522_setBitReg(uint8_t *Addr,uint8_t Mask)
{
	/*
	@brief: set MFRC522 reg address
	@steps:read value of reg ---> set bits ---> write value of reg
	
	@Addr: register address
	@Mask: value of mask
	
	@return: HAL_OK,HAL_ERROR
	*/

	uint8_t re_Data=0,wr_Data=0;
	//read value of register
	if (MFRC522_readReg(Addr,&re_Data,1)!=HAL_OK)
		return HAL_ERROR;
	//set bits 
	wr_Data=re_Data|Mask;
	//write value of register
	if (MFRC522_writeReg(Addr,&wr_Data,1)!=HAL_OK)
		return HAL_ERROR;
	
	return HAL_OK;
}

HAL_StatusTypeDef MFRC522_clearBitReg(uint8_t *Addr,uint8_t Mask)
{
	/*
	@brief: set MFRC522 reg address
	@steps:read value of reg ---> clear bits ---> write value of reg
	
	@Addr: register address
	@Mask: value of mask
	
	@return: HAL_OK,HAL_ERROR
	*/

	uint8_t re_Data=0,wr_Data=0;
	//read value of register
	if (MFRC522_readReg(&Addr,&re_Data,1)!=HAL_OK)
		return HAL_ERROR;
	//clear bit 
	wr_Data=re_Data&(~Mask);
	//write value of register
	if (MFRC522_writeReg(&Addr,&wr_Data,1)!=HAL_OK)
		return HAL_ERROR;
	
	return HAL_OK;
}	
	
HAL_StatusTypeDef MFRC522_readReg(uint8_t *Addr,uint8_t *Data,uint8_t Size)
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
	Addr_Data[0]=((*Addr<<1)&0x7E)|0x80;
	

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

HAL_StatusTypeDef MFRC522_writeReg(uint8_t *Addr,uint8_t *Data,uint8_t Size)
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