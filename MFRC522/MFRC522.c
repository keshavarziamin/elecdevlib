#include "MFRC522.h"

MFRC522_HandleTypeDef MF_typedef;


//private functions
static void MFRC522_readData(void);
static void MFRC522_writeData(void);
void MFRC522_init(MFRC522_HandleTypeDef MF_config)
{
	/*
	@braif: initializing the MFRC522
					1.set hardwre port to connect to device
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
	MFRC522_ChipSelect_ON();
	MFRC522_ChipSelect_OFF();
}

void MFRC522_setDefault(void)
{
	
}

static void MFRC522_readData(void)
{
}

static void MFRC522_writeData(void)
{
}