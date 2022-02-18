#include "FlashMemory.h"



static void ENABLE_CS(FLASH_MEM_HandleTypedef *FM);
static void DISABLE_CS(FLASH_MEM_HandleTypedef *FM);
static void ENABLE_WP(FLASH_MEM_HandleTypedef *FM);
static void DISABLE_WP(FLASH_MEM_HandleTypedef *FM);
static void ENABLE_RST(FLASH_MEM_HandleTypedef *FM);
static void DISABLE_RST(FLASH_MEM_HandleTypedef *FM);

uint32_t flash_error;

HAL_StatusTypeDef FLASH_init(FLASH_MEM_HandleTypedef *FM)
{
	uint8_t information[4],status;
	
	DISABLE_CS(FM);//set 1 chip selcet pin
	DISABLE_RST(FM);//
	//ENABLE_WP(FM);
	DISABLE_WP(FM);
	//get information
	FLASH_getInformation(FM,information);
	FLASH_disableSectorProtection(FM);
	HAL_Delay(200);
	FLASH_readStatusRegister(FM,&status);
	printf("status rtegister is %x\r\n",status);
	FM->Register.Index.read=0;
	FM->Register.Index.write=0;
}

HAL_StatusTypeDef FLASH_writeFIFO(FLASH_MEM_HandleTypedef *FM,void* Data,uint8_t sizeOfData)
{
	//caculate index
	uint16_t byteAddr=0x00;
	uint16_t pageAddr=(uint16_t)FM->Register.Index.write%MAX_WRITE_DATA;
	FM->Register.Index.write++;
	
	if (FM->Register.Index.count<MAX_WRITE_DATA)
		FM->Register.Index.count++;
	else
		FM->Register.Index.read=FM->Register.Index.write; //
	
	
	return FLASH_writeData(FM,pageAddr,byteAddr,Data,sizeOfData);
}

HAL_StatusTypeDef FLASH_readFIFO(FLASH_MEM_HandleTypedef *FM,void* Data,uint8_t sizeOfData)
{
	//caculate index
	uint16_t byteAddr=0x00;
	uint16_t pageAddr=(uint16_t)FM->Register.Index.read%MAX_WRITE_DATA;
	//FM->Register.Index.read++;
	
	
	return FLASH_readData(FM,pageAddr,byteAddr,Data,sizeOfData);
}


HAL_StatusTypeDef FLASH_readStatusRegister(FLASH_MEM_HandleTypedef *FM,uint8_t *SR)
{
	uint8_t CMD=OPCODE_READ_STATUS_REGISTER;
	
	 if (FLASH_getData(FM,&CMD,1,SR,1)!=HAL_OK)
		 return HAL_ERROR;
	 
	 FM->Register.statusRegister=*SR;
	 return HAL_OK;
	 
	 
}
HAL_StatusTypeDef FLASH_getInformation(FLASH_MEM_HandleTypedef *FM,uint8_t *info)
{
	uint8_t CMD=OPCODE_READ_DEVICE_ID;
	
	if (FLASH_getData(FM,&CMD,1,info,4)!=HAL_OK)
		return HAL_ERROR;
	
	FM->Register.information=((info[0]<<8)&0xFF00)|(info[1]);
	return HAL_OK;
	
}



HAL_StatusTypeDef FLASH_writeData(FLASH_MEM_HandleTypedef *FM,uint16_t pageAddr,uint16_t byteAddr,uint8_t *TxData,uint8_t lengthOfTx)
{
	uint8_t const lengthOfTXAddr=3;
 
	uint8_t statusRegister,CMDTX[4];
	HAL_StatusTypeDef StatusTx;

	
	union TX_ADDRESS TxAddr;
	TxAddr.word=0;
	TxAddr.word|=(uint32_t)(byteAddr&0x03FF);
	TxAddr.word|=(uint32_t)((pageAddr<<10)&0x0003FFC00);
	
	//check ready state to write data
	if(FLASH_readStatusRegister(FM,&statusRegister)!=HAL_OK)
		return HAL_ERROR;
	if ((statusRegister&STATUS_REGISTER_READY)!=STATUS_REGISTER_READY)
		return HAL_BUSY;
	//write buffer
	uint8_t lengthOfTXBuffer=lengthOfTXAddr+lengthOfTx+1;
	uint8_t *TxBuffer=malloc(lengthOfTXBuffer*sizeof(uint8_t));
	
	
	CMDTX[0]=OPCODE_WRITE_BUFFER_1;//add CMD
	for(uint8_t k=1;k<4;k++)
		CMDTX[k]=TxAddr.byte[3-k];
	
	memcpy(TxBuffer,CMDTX,4);	
	memcpy(&TxBuffer[1+lengthOfTXAddr],TxData,lengthOfTx);//add Data;
	
	StatusTx=FLASH_sendData(FM,TxBuffer,lengthOfTXBuffer);
	
	memset(TxBuffer,0,lengthOfTXBuffer);
	free(TxBuffer);
	
	if (StatusTx!=HAL_OK)
		return HAL_ERROR;
	
	//write buffer to MMP
	CMDTX[0]=OPCODE_PROGRAM_BUFFER1_TO_MMP_WITH_BUILT_IN; //change CMD. erase MMP before write it
	if(FLASH_sendData(FM,CMDTX,(lengthOfTXAddr+1))!=HAL_OK)
		return HAL_ERROR;
	
	printf("write data to flash memory (page addr = %d)\r\n",pageAddr);
	return HAL_OK;
	
}

HAL_StatusTypeDef FLASH_readData(FLASH_MEM_HandleTypedef *FM,uint16_t pageAddr,uint16_t byteAddr,uint8_t *RxData,uint8_t lengthOfRx)
{
	uint8_t const lengthOfTXAddr=3;
 
	uint8_t statusRegister,CMDTX[8]={0};
	HAL_StatusTypeDef StatusTx;


	
	
	union TX_ADDRESS TxAddr;
	TxAddr.word=0;
	TxAddr.word|=(uint32_t)(byteAddr&0x03FF);
	TxAddr.word|=(uint32_t)((pageAddr<<10)&0x0003FFC00);
	
	//check ready state to write data
	if(FLASH_readStatusRegister(FM,&statusRegister)!=HAL_OK)
		return HAL_ERROR;
	if ((statusRegister&STATUS_REGISTER_READY)!=STATUS_REGISTER_READY)
		return HAL_BUSY;
	
	
	
	CMDTX[0]=OPCODE_READ_MAIN_MEMORY_PAGE;//add CMD
	for(uint8_t k=1;k<4;k++)
		CMDTX[k]=TxAddr.byte[3-k];
	

	if(FLASH_getData(FM,CMDTX,8,RxData,lengthOfRx)!=HAL_OK)
		return HAL_ERROR;
	
	printf("read data from flash memory (page addr = %d)(length of Rx = %d)\r\n",pageAddr,lengthOfRx);

	return HAL_OK;
		
}

////////low level functions///////////////////////////////////////
HAL_StatusTypeDef FLASH_sendData(FLASH_MEM_HandleTypedef *FM,uint8_t *transmitData,uint16_t lengthOfTX)
{
	HAL_StatusTypeDef ST;
		uint8_t statusRegister;
	
		
	
	DISABLE_WP(FM);
	ENABLE_CS(FM);
	

	ST= HAL_SPI_Transmit(&FM->Config.spi,transmitData,lengthOfTX,100);
	
	DISABLE_CS(FM);
	ENABLE_WP(FM);
	show_hex("transData=> ",transmitData,lengthOfTX);
	return ST;
}
HAL_StatusTypeDef FLASH_getData(FLASH_MEM_HandleTypedef *FM,uint8_t *transmitData,uint16_t lengthOfTX,uint8_t *receivedData,uint16_t lengthOfRX)
{
	uint8_t timeout=10;
	uint16_t lengthOfTxBuffer=lengthOfTX;
	uint16_t lengthOfRxBuffer=lengthOfRX+lengthOfTxBuffer;
	
	HAL_StatusTypeDef status;
	uint8_t *RxBuffer=malloc(lengthOfRxBuffer*sizeof(uint8_t));	

	
	//get data
	ENABLE_CS(FM);
	while(timeout--){
	status=HAL_SPI_TransmitReceive(&FM->Config.spi,transmitData,RxBuffer,lengthOfRxBuffer,100);
	if (status==HAL_OK){
		memcpy(receivedData,&RxBuffer[lengthOfTX],lengthOfRX); 
		printf("status of getData is %x  and length of RX is %d\r\n",status,lengthOfRX);
		show_hex("recvData=> ",receivedData,lengthOfRX);
		flash_error&=~ERR_FLASH_HW;  //set bit error
		break;
	}
}
		DISABLE_CS(FM);
	//clear buffer
	memset(RxBuffer,0,lengthOfRxBuffer);
	free(RxBuffer);

	if (status!=HAL_OK)
		flash_error|=ERR_FLASH_HW; //clear bit error
	
	return status;
}


HAL_StatusTypeDef FLASH_enableSectorProtection(FLASH_MEM_HandleTypedef *FM)
{
	union TX_ADDRESS CMDTX;
	CMDTX.word=OPCODE_ENABLE_SECTOR_PROTECTION;
	
	return FLASH_sendData(FM,CMDTX.byte,4);
}

HAL_StatusTypeDef FLASH_disableSectorProtection(FLASH_MEM_HandleTypedef *FM)
{
	union TX_ADDRESS CMDTX;
	CMDTX.word=OPCODE_DISABLE_SECTOR_PROTECTION;
	
	return FLASH_sendData(FM,CMDTX.byte,4);
}


uint32_t Flash_getError(void){
	return flash_error;
}

/////static functions/////////////////////
static void ENABLE_CS(FLASH_MEM_HandleTypedef *FM)
{
	 HAL_GPIO_WritePin(FM->Config.CS.GPIOx,FM->Config.CS.Pin,GPIO_PIN_RESET);
}

static void DISABLE_CS(FLASH_MEM_HandleTypedef *FM)
{
	 HAL_GPIO_WritePin(FM->Config.CS.GPIOx,FM->Config.CS.Pin,GPIO_PIN_SET);
}


static void ENABLE_WP(FLASH_MEM_HandleTypedef *FM)
{
	 HAL_GPIO_WritePin(FM->Config.WP.GPIOx,FM->Config.WP.Pin,GPIO_PIN_SET);
}

static void DISABLE_WP(FLASH_MEM_HandleTypedef *FM)
{
	 HAL_GPIO_WritePin(FM->Config.WP.GPIOx,FM->Config.WP.Pin,GPIO_PIN_RESET);
}

static void ENABLE_RST(FLASH_MEM_HandleTypedef *FM)
{
	 HAL_GPIO_WritePin(FM->Config.WP.GPIOx,FM->Config.WP.Pin,GPIO_PIN_RESET);
}

static void DISABLE_RST(FLASH_MEM_HandleTypedef *FM)
{
	 HAL_GPIO_WritePin(FM->Config.WP.GPIOx,FM->Config.WP.Pin,GPIO_PIN_SET);
}


